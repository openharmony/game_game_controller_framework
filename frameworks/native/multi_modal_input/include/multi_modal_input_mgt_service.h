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
#ifndef GAME_CONTROLLER_MULTI_MODAL_INPUT_MGT_SERVICE_H
#define GAME_CONTROLLER_MULTI_MODAL_INPUT_MGT_SERVICE_H

#include <condition_variable>
#include "singleton.h"
#include "input_device_listener.h"
#include "gamecontroller_constants.h"
#include "gamecontroller_client_model.h"
#include "device_info_service.h"
#include "ffrt.h"

namespace OHOS {
namespace GameController {
using namespace OHOS::MMI;

struct DeviceChangeEvent {
    int32_t deviceId;
    DeviceChangeType deviceChangeType;
};

class MultiModalInputMgtService : public DelayedSingleton<MultiModalInputMgtService> {
DECLARE_DELAYED_SINGLETON(MultiModalInputMgtService)

public:
    /**
     * Get All Devices when call RegisterGameDeviceEventCallback
     */
    virtual void GetAllDeviceInfosWhenRegisterDeviceMonitor();

    /**
     * Get All Devices
     * @return first indicates the query result, and second indicates the information of all devices.
     */
    virtual std::pair<int32_t, std::vector<DeviceInfo>> GetAllDeviceInfos();

    /**
     * Queries device information from the cache based on the device ID.
     * @param deviceId Device ID.
     * @return Device Information
     */
    virtual DeviceInfo GetDeviceInfo(const int32_t deviceId);

    /**
     * Queries device information from the cache based on the device uniq.
     * @param uniq device's uniq
     * @return Device Information
     */
    virtual DeviceInfo GetDeviceInfoByUniq(const std::string &uniq);

    /**
     * The same device has multiple device status change events. Therefore, the processing needs to be delayed.
     * @param deviceChangeEvent Device change event
     */
    virtual void DelayHandleDeviceChangeEvent(const DeviceChangeEvent &deviceChangeEvent);

private:
    /**
     * Executing a Device Event Callback
     * @param deviceInfo Device Information
     * @param deviceChangeType Device Change Type
     */
    void DoDeviceEventCallback(const DeviceInfo &deviceInfo, const DeviceChangeType &deviceChangeType);

    /**
     * Handling Device Delay Events
     */
    void HandleDeviceChangeEvent();

    /**
     * Processing Device Offline Events
     * @param deviceId Device ID.
     * @param tempDeviceIdUniqMap Map of the temporary device ID and uniq
     * @param tempDeviceInfoByUniqMap Temporary device information map
     */
    void HandleDeviceRemoveEvent(const int32_t deviceId, std::unordered_map<int32_t, std::string> &tempDeviceIdUniqMap,
                                 std::unordered_map<std::string, DeviceInfo> &tempDeviceInfoByUniqMap);

    /**
     * Processing Device Online Events
     * @param deviceId Device ID.
     * @param tempDeviceIdUniqMap Map of the temporary device ID and uniq
     * @param tempDeviceInfoByUniqMap Temporary device information map
     */
    void HandleDeviceAddEvent(const int32_t deviceId, std::unordered_map<int32_t, std::string> &tempDeviceIdUniqMap,
                              std::unordered_map<std::string, DeviceInfo> &tempDeviceInfoByUniqMap);

    /**
     * if uniq is empty, query uniq from tempDeviceInfoByUniqMap or deviceInfoByUniqMap_ by product and vendor
     * @param tempDeviceInfoByUniqMap Temporary device information map
     * @param inputDeviceInfo deviceInfo from multi-input
     * @return true means uniq is not empty
     */
    bool GetUniqOnDeviceAddEvent(const std::unordered_map<std::string, DeviceInfo> &tempDeviceInfoByUniqMap,
                                 InputDeviceInfo &inputDeviceInfo);

    /**
     * Identifying the Device Type
     * @param tempDeviceInfoByUniqMap Map of the temporary device ID and uniq
     * @param oprType Operation type. The value 0 indicates that device identification is performed
     * when all device information is queried. 1: The device is identified when the device goes online.
     */
    void IdentifyDeviceType(std::unordered_map<std::string, DeviceInfo> &tempDeviceInfoByUniqMap, int oprType);

    /**
     * Convert InputDeviceInfo to DeviceInfo and place it in tempDeviceInfoByUniqMap.
     * @param tempDeviceInfoByUniqMap Map of the temporary device ID and uniq
     * @param inputDeviceInfo InputDeviceInfo
     */
    void HandleInputDeviceInfo(std::unordered_map<std::string, DeviceInfo> &tempDeviceInfoByUniqMap,
                               InputDeviceInfo &inputDeviceInfo);

    /**
     * Clearing Offline Devices
     * @param tempDeviceInfoByUniqMap Temporary device information map
     */
    void CleanOfflineDevice(const std::unordered_map<std::string, DeviceInfo> &tempDeviceInfoByUniqMap);

    /**
     * Clear from deviceIdUniqMap_ by deviceId
     * @param deviceId deviceId
     */
    void ClearDeviceIdUniqMapByDeviceId(int32_t deviceId);

    /**
     * Clear deviceIdUniqMap_
     */
    void ClearAllDeviceIdUniqMap();

    int64_t GetSysClockTime();

private:

    /**
     * Device Information Cache
     * The key is uniq.
     * The value is device information.
     */
    std::unordered_map<std::string, DeviceInfo> deviceInfoByUniqMap_;

    /**
     * Relationship between deviceId and uniq
     * The key is deviceId.
     * The Value is uniq.
     */
    std::unordered_map<int32_t, std::string> deviceIdUniqMap_;

    /**
     * Device change event lock
     */
    std::mutex deviceChangeEventMutex_;

    /**
     * Cached device change events
     */
    std::vector<DeviceChangeEvent> deviceChangeEventCache_;

    /**
     * Indicates whether to enable delayed handle.
     */
    bool needStartDelayHandle_{true};

    /**
     * Device online and offline processing queue
     */
    std::unique_ptr<ffrt::queue> deviceTaskQueue_{nullptr};

    /**
     * Event callback queue
     */
    std::unique_ptr<ffrt::queue> eventCallbackQueue_{nullptr};
};
}
}
#endif //GAME_CONTROLLER_MULTI_MODAL_INPUT_MGT_SERVICE_H
