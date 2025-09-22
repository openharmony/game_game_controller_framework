/*
 *  Copyright (c) 2025 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <input_manager.h>
#include "multi_modal_input_mgt_service.h"
#include "gamecontroller_log.h"
#include "device_identify_service.h"
#include "device_event_callback.h"
#include "gamecontroller_errors.h"
#include "window_input_intercept.h"
#include "key_mapping_service.h"
#include "gamecontroller_utils.h"

namespace OHOS {
namespace GameController {
using namespace OHOS::MMI;
namespace {
/**
 * Delay duration: 100 ms
 */
constexpr int64_t DELAY_TIME = 100 * 1000;

/**
 * Querying Information About All Devices
 */
const int32_t OPR_TYPE_GET_ALL_DEVICES = 0;

/**
 * Devices go online.
 */
const int32_t OPR_TYPE_DEVICE_ONLINE = 1;

}

MultiModalInputMgtService::MultiModalInputMgtService()
{
    deviceTaskQueue_ = std::make_unique<ffrt::queue>("deviceTaskQueue",
                                                     ffrt::queue_attr().qos(ffrt::qos_default));
    eventCallbackQueue_ = std::make_unique<ffrt::queue>("deiceEventCallbackQueue",
                                                        ffrt::queue_attr().qos(ffrt::qos_default));
}

MultiModalInputMgtService::~MultiModalInputMgtService()
{
}

void MultiModalInputMgtService::GetAllDeviceInfosWhenRegisterDeviceMonitor()
{
    bool isNeedGetAllDeviceInfos = false;
    {
        std::lock_guard<ffrt::mutex> lock(deviceChangeEventMutex_);
        if (deviceInfoByUniqMap_.empty()) {
            isNeedGetAllDeviceInfos = true;
        } else {
            for (const auto &pair: deviceInfoByUniqMap_) {
                DoDeviceEventCallback(pair.second, ADD);
            }
        }
    }
    if (isNeedGetAllDeviceInfos) {
        GetAllDeviceInfos();
    }
}

std::pair<int32_t, std::vector<DeviceInfo>> MultiModalInputMgtService::GetAllDeviceInfos()
{
    std::lock_guard<ffrt::mutex> lock(deviceChangeEventMutex_);
    std::pair<int32_t, std::vector<DeviceInfo>> result;
    std::pair<int32_t, std::vector<InputDeviceInfo>> inputDeviceInfosPair =
        DelayedSingleton<DeviceInfoService>::GetInstance()->GetAllDeviceInfos();
    result.first = inputDeviceInfosPair.first;
    if (result.first != GAME_CONTROLLER_SUCCESS) {
        HILOGW("[GameController]GetAllDeviceInfos failed. the error is %{public}d", result.first);
        return result;
    }

    /**
     * The same physical device has multiple original device information InputDeviceInfo,
     * and the uniq of the same device is the same. Therefore, the device needs to be aggregated into
     * the DeviceInfo structure based on the uniq.
     */
    std::unordered_map<std::string, DeviceInfo> deviceInfoByUniqMap;
    for (auto inputDeviceInfo: inputDeviceInfosPair.second) {
        HandleInputDeviceInfo(deviceInfoByUniqMap, inputDeviceInfo);
    }

    /**
     * 1. Perform device identification.
     * 2. Sends online notifications to new devices or devices whose device types are changed.
     */
    IdentifyDeviceType(deviceInfoByUniqMap, OPR_TYPE_GET_ALL_DEVICES);

    /**
     * Sends offline notifications to devices that do not exist
     * and deletes devices that do not exist from the local cache.
     */
    CleanOfflineDevice(deviceInfoByUniqMap);

    std::vector<DeviceInfo> deviceInfos;
    for (const auto &pair: deviceInfoByUniqMap) {
        deviceInfos.push_back(pair.second);
    }
    result.second = deviceInfos;
    HILOGI("[GameController]GetAllDeviceInfos success.");
    return result;
}

void MultiModalInputMgtService::DoDeviceEventCallback(const DeviceInfo &deviceInfo,
                                                      const DeviceChangeType &deviceChangeType)
{
    eventCallbackQueue_->submit([deviceInfo, deviceChangeType, this] {
        DeviceEvent deviceEvent;
        deviceEvent.deviceInfo = deviceInfo;
        deviceEvent.changeType = deviceChangeType;
        DelayedSingleton<DeviceEventCallback>::GetInstance()->OnDeviceEventCallback(deviceEvent);
    });
}

DeviceInfo MultiModalInputMgtService::GetDeviceInfo(const int32_t deviceId)
{
    std::lock_guard<ffrt::mutex> lock(deviceChangeEventMutex_);
    if (deviceIdUniqMap_.find(deviceId) == deviceIdUniqMap_.end()) {
        HILOGW("[GameController]GetDeviceInfo failed. No Uniq for deviceId[%{public}d]", deviceId);
        return DeviceInfo();
    }
    std::string uniq = deviceIdUniqMap_[deviceId];
    if (deviceInfoByUniqMap_.find(uniq) == deviceInfoByUniqMap_.end()) {
        // If an exception occurs, delete it from deviceInfoByUniqMap_.
        ClearDeviceIdUniqMapByDeviceId(deviceId);
        HILOGW("[GameController]GetDeviceInfo failed. No deviceInfo for deviceId[%{public}d]", deviceId);
        return DeviceInfo();
    }
    return deviceInfoByUniqMap_[uniq];
}

DeviceInfo MultiModalInputMgtService::GetDeviceInfoByUniq(const std::string &uniq)
{
    std::lock_guard<ffrt::mutex> lock(deviceChangeEventMutex_);
    if (deviceInfoByUniqMap_.find(uniq) == deviceInfoByUniqMap_.end()) {
        return DeviceInfo();
    }
    return deviceInfoByUniqMap_[uniq];
}

void MultiModalInputMgtService::DelayHandleDeviceChangeEvent(const DeviceChangeEvent &deviceChangeEvent)
{
    std::lock_guard<ffrt::mutex> lock(deviceChangeEventMutex_);
    deviceChangeEventCache_.push_back(deviceChangeEvent);

    if (needStartDelayHandle_) {
        // Start Delayed Aggregation Processing
        needStartDelayHandle_ = false;
        ffrt::task_attr taskAttr;
        taskAttr.delay(DELAY_TIME);
        ffrt::task_handle taskHandle = deviceTaskQueue_->submit_h([this] { HandleDeviceChangeEvent(); }, taskAttr);
        if (taskHandle == nullptr) {
            HILOGW("[GameController]DelayHandleDeviceChangeEvent submit queue fail");
            needStartDelayHandle_ = true;
        } else {
            HILOGI("[GameController]DelayHandleDeviceChangeEvent task submit queue success");
        }
    }
}

void MultiModalInputMgtService::HandleDeviceChangeEvent()
{
    std::lock_guard<ffrt::mutex> lock(deviceChangeEventMutex_);
    std::unordered_map<int32_t, std::string> tempDeviceIdUniqMap;
    std::unordered_map<std::string, DeviceInfo> tempDeviceInfoByUniqMap;
    for (auto deviceChangeEvent: deviceChangeEventCache_) {
        if (deviceChangeEvent.deviceChangeType == DeviceChangeType::REMOVE) {
            // Device offline
            HILOGI("[GameController]begin HandleDeviceRemoveEvent. deviceId[%{public}d]", deviceChangeEvent.deviceId);
            HandleDeviceRemoveEvent(deviceChangeEvent.deviceId, tempDeviceIdUniqMap, tempDeviceInfoByUniqMap);
            continue;
        }
        HILOGI("[GameController]begin HandleDeviceAddEvent. deviceId[%{public}d]", deviceChangeEvent.deviceId);
        HandleDeviceAddEvent(deviceChangeEvent.deviceId, tempDeviceIdUniqMap, tempDeviceInfoByUniqMap);
    }

    if (!tempDeviceInfoByUniqMap.empty()) {
        // Identifies devices and sends device online events.
        IdentifyDeviceType(tempDeviceInfoByUniqMap, OPR_TYPE_DEVICE_ONLINE);
    }

    deviceChangeEventCache_.clear();
    needStartDelayHandle_ = true;
}

void MultiModalInputMgtService::HandleDeviceRemoveEvent(
    const int32_t deviceId,
    std::unordered_map<int32_t, std::string> &tempDeviceIdUniqMap,
    std::unordered_map<std::string, DeviceInfo> &tempDeviceInfoByUniqMap)
{
    if (deviceIdUniqMap_.find(deviceId) == deviceIdUniqMap_.end()) {
        // No online information about the device is received.
        if (tempDeviceIdUniqMap.find(deviceId) == tempDeviceIdUniqMap.end()) {
            // The device information corresponding to the device is not found. Therefore, the event is discarded.
            HILOGI("[GameController]Discard DeviceOfflineEvent. Because no receive online event."
                   " deviceId is %{public}d", deviceId);
            return;
        }

        HILOGI("[GameController]Discard DeviceOfflineEvent. Clear tempDeviceIdUniqMap and tempDeviceInfoByUniqMap."
               " deviceId is %{public}d", deviceId);

        // If device information exists in the temporary cache, the device information is deleted and discarded.
        std::string uniq = tempDeviceIdUniqMap[deviceId];
        tempDeviceIdUniqMap.erase(deviceId);
        if (tempDeviceInfoByUniqMap.find(uniq) == tempDeviceInfoByUniqMap.end()) {
            return;
        }

        // Delete other device IDs associated with the device.
        DeviceInfo deviceInfo = tempDeviceInfoByUniqMap[uniq];
        for (auto id: deviceInfo.ids) {
            tempDeviceIdUniqMap.erase(id);
        }
        tempDeviceInfoByUniqMap.erase(uniq);
        return;
    }

    ClearOfflineDeviceAndBroadcast(deviceId);
}

void MultiModalInputMgtService::HandleDeviceAddEvent(
    const int32_t deviceId,
    std::unordered_map<int32_t, std::string> &tempDeviceIdUniqMap,
    std::unordered_map<std::string, DeviceInfo> &tempDeviceInfoByUniqMap)
{
    std::pair<int32_t, InputDeviceInfo> pair =
        DelayedSingleton<DeviceInfoService>::GetInstance()->GetInputDeviceInfo(deviceId);
    InputDeviceInfo inputDeviceInfo = pair.second;
    if (pair.first != GAME_CONTROLLER_SUCCESS) {
        HILOGW("[GameController]Discard DeviceOnlineEvent. deviceId is %{public}d, because GetInputDeviceInfo failed",
               deviceId);
        return;
    }

    if (!GetUniqOnDeviceAddEvent(tempDeviceInfoByUniqMap, inputDeviceInfo)) {
        return;
    }

    tempDeviceIdUniqMap[deviceId] = inputDeviceInfo.uniq;
    HandleInputDeviceInfo(tempDeviceInfoByUniqMap, inputDeviceInfo);
}

bool MultiModalInputMgtService::GetUniqOnDeviceAddEvent(
    const std::unordered_map<std::string, DeviceInfo> &tempDeviceInfoByUniqMap,
    InputDeviceInfo &inputDeviceInfo)
{
    if (!inputDeviceInfo.UniqIsEmpty()) {
        return true;
    }

    // uniq is empty in the virtual device information
    if (!inputDeviceInfo.IsVirtualDeviceForExternalDevice()) {
        // The system's own virtual devices do not need to be notified to the user.
        HILOGW("[GameController]Discard DeviceOnlineEvent. deviceId is %{public}d, "
               "because it's system virtual device",
               inputDeviceInfo.id);
        return false;
    }

    inputDeviceInfo.uniq = std::to_string(inputDeviceInfo.vendor) + "_"
        + std::to_string(inputDeviceInfo.product);
    HILOGI("[GameController] HandleDeviceAddEvent: Embeded device. "
           "deviceId is %{public}d, vendor is %{public}d, product is %{public}d. "
           "Because uniq is empty,construct a new uniq. ",
           inputDeviceInfo.id, inputDeviceInfo.vendor, inputDeviceInfo.product);
    return true;
}

void MultiModalInputMgtService::HandleInputDeviceInfo(
    std::unordered_map<std::string, DeviceInfo> &tempDeviceInfoByUniqMap,
    InputDeviceInfo &inputDeviceInfo)
{
    // Aggregation by uniq
    if (tempDeviceInfoByUniqMap.find(inputDeviceInfo.uniq) == tempDeviceInfoByUniqMap.end()) {
        DeviceInfo deviceInfo;
        deviceInfo.ids.insert(inputDeviceInfo.id);
        deviceInfo.uniq = inputDeviceInfo.uniq;
        deviceInfo.phys = inputDeviceInfo.phys;
        deviceInfo.product = inputDeviceInfo.product;
        deviceInfo.version = inputDeviceInfo.version;
        deviceInfo.name = inputDeviceInfo.name;
        deviceInfo.names.insert(inputDeviceInfo.name);
        deviceInfo.vendor = inputDeviceInfo.vendor;
        for (auto &sourceType: inputDeviceInfo.sourceTypeSet) {
            deviceInfo.sourceTypeSet.insert(sourceType);
        }
        deviceInfo.idSourceTypeMap[inputDeviceInfo.id] = inputDeviceInfo.sourceTypeSet;
        tempDeviceInfoByUniqMap[inputDeviceInfo.uniq] = deviceInfo;
    } else {
        DeviceInfo deviceInfo = tempDeviceInfoByUniqMap[inputDeviceInfo.uniq];
        deviceInfo.ids.insert(inputDeviceInfo.id);
        deviceInfo.name = inputDeviceInfo.name;
        deviceInfo.names.insert(inputDeviceInfo.name);
        for (auto &sourceType: inputDeviceInfo.sourceTypeSet) {
            deviceInfo.sourceTypeSet.insert(sourceType);
        }
        deviceInfo.idSourceTypeMap[inputDeviceInfo.id] = inputDeviceInfo.sourceTypeSet;
        tempDeviceInfoByUniqMap[inputDeviceInfo.uniq] = deviceInfo;
    }
}

void MultiModalInputMgtService::IdentifyDeviceType(
    std::unordered_map<std::string, DeviceInfo> &tempDeviceInfoByUniqMap, int32_t oprType)
{
    std::vector<DeviceInfo> deviceInfos;
    for (const auto &pair: tempDeviceInfoByUniqMap) {
        deviceInfos.push_back(pair.second);
    }
    std::vector<DeviceInfo> result = DelayedSingleton<DeviceIdentifyService>::GetInstance()->IdentifyDeviceType(
        deviceInfos);
    if (oprType == OPR_TYPE_GET_ALL_DEVICES) {
        /*
         * When all device information is queried, the local cache is fully updated.
         * Therefore, device_id_uniq_Map_ needs to be cleared.
         */
        ClearAllDeviceIdUniqMap();
    }

    bool isNeedNotify; // 需要对本地缓存新增或更新设备类型时，需要发送一个设备上线事件
    for (auto deviceInfo: result) {
        if (deviceInfoByUniqMap_.find(deviceInfo.uniq) == deviceInfoByUniqMap_.end()) {
            // The local cache does not exist. The online event needs to be sent.
            isNeedNotify = true;
            deviceInfo.onlineTime = StringUtils::GetSysClockTime();
        } else {
            if (deviceInfoByUniqMap_[deviceInfo.uniq].deviceType == deviceInfo.deviceType) {
                isNeedNotify = false;
            } else {
                // When the device type changes, a notification needs to be sent to go online.
                isNeedNotify = true;
            }
        }
        deviceInfo.anonymizationUniq = StringUtils::AnonymizationUniq(deviceInfo.uniq);
        deviceInfo.vidPid = deviceInfo.GetVidPid();
        deviceInfoByUniqMap_[deviceInfo.uniq] = deviceInfo;
        tempDeviceInfoByUniqMap[deviceInfo.uniq] = deviceInfo;
        for (auto id: deviceInfo.ids) {
            deviceIdUniqMap_[id] = deviceInfo.uniq;
            DelayedSingleton<WindowInputIntercept>::GetInstance()->RegisterWindowInputIntercept(id);
        }
        if (isNeedNotify) {
            HILOGI("[GameController]DeviceOnlineEvent. oprType is %{public}d, deviceInfo is %{public}s",
                   oprType, deviceInfo.GetDeviceInfoDesc().c_str());
            DelayedSingleton<KeyMappingService>::GetInstance()->BroadCastDeviceInfo(deviceInfo);
            DoDeviceEventCallback(deviceInfo, ADD);
        }
    }
}

void MultiModalInputMgtService::CleanOfflineDevice(
    const std::unordered_map<std::string, DeviceInfo> &tempDeviceInfoByUniqMap)
{
    std::vector<std::string> needDelete;
    for (auto &pair: deviceInfoByUniqMap_) {
        if (tempDeviceInfoByUniqMap.find(pair.first) != tempDeviceInfoByUniqMap.end()) {
            continue;
        }
        HILOGI("[GameController][CleanOfflineDevice]DeviceOfflineEvent. DeviceInfo is %{public}s",
               pair.second.GetDeviceInfoDesc().c_str());
        DoDeviceEventCallback(pair.second, REMOVE);
        needDelete.push_back(pair.first);
    }

    for (auto uniq: needDelete) {
        deviceInfoByUniqMap_.erase(uniq);
    }
}

void MultiModalInputMgtService::ClearDeviceIdUniqMapByDeviceId(int32_t deviceId)
{
    deviceIdUniqMap_.erase(deviceId);
    DelayedSingleton<WindowInputIntercept>::GetInstance()->UnRegisterWindowInputIntercept(deviceId);
}

void MultiModalInputMgtService::ClearAllDeviceIdUniqMap()
{
    deviceIdUniqMap_.clear();
    DelayedSingleton<WindowInputIntercept>::GetInstance()->UnRegisterAllWindowInputIntercept();
}

void MultiModalInputMgtService::ClearOfflineDeviceAndBroadcast(const int32_t deviceId){
    std::string uniq = deviceIdUniqMap_[deviceId];
    ClearDeviceIdUniqMapByDeviceId(deviceId);
    if (deviceInfoByUniqMap_.find(uniq) == deviceInfoByUniqMap_.end()) {
        HILOGW("[GameController]Discard DeviceOfflineEvent. Because no deviceInfo. "
               "deviceId is %{public}d, uniq is %{public}s", deviceId, StringUtils::AnonymizationUniq(uniq).c_str());
        return;
    }
    DeviceInfo deviceInfo = deviceInfoByUniqMap_[uniq];
    for (auto id: deviceInfo.ids) {
        // Delete other device IDs associated with the device.
        ClearDeviceIdUniqMapByDeviceId(id);
    }
    deviceInfoByUniqMap_.erase(uniq);
    HILOGI("[GameController][ClearOfflineDeviceAndBroadcast]DeviceOfflineEvent. DeviceInfo is %{public}s",
           deviceInfo.GetDeviceInfoDesc().c_str());
    deviceInfo.status = 1;

    // Broadcast device offline
    DelayedSingleton<KeyMappingService>::GetInstance()->BroadCastDeviceInfo(deviceInfo);

    DoDeviceEventCallback(deviceInfo, DeviceChangeType::REMOVE);
}

}
}