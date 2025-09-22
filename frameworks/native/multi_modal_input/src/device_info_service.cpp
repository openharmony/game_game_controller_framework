/*
 *  Copyright (c) 2025 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */


#include "device_info_service.h"
#include "gamecontroller_errors.h"
#include <input_manager.h>
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {

using namespace OHOS::MMI;

namespace {
/*
 * Timeout interval for querying device information: 500 ms
 */
const int32_t QUERY_DEVICE_TIMEOUT_MS = 500;

InputSourceType g_inputSourceType[] = {
    {KEYBOARD,    EVDEV_UDEV_TAG_KEYBOARD},
    {MOUSE,       EVDEV_UDEV_TAG_MOUSE},
    {TOUCHPAD,    EVDEV_UDEV_TAG_TOUCHPAD},
    {TOUCHSCREEN, EVDEV_UDEV_TAG_TOUCHSCREEN},
    {JOYSTICK,    EVDEV_UDEV_TAG_JOYSTICK},
    {TRACKBALL,   EVDEV_UDEV_TAG_TRACKBALL},
};
}

DeviceInfoService::DeviceInfoService()
{
}

DeviceInfoService::~DeviceInfoService()
{
}

std::pair<int32_t, InputDeviceInfo> DeviceInfoService::GetInputDeviceInfo(int32_t deviceId)
{
    std::unique_lock<std::mutex> lock(taskMutex_);
    inputDevice_ = nullptr;
    taskFinish_ = false;
    int result = InputManager::GetInstance()->GetDevice(deviceId, [&](std::shared_ptr<InputDevice> inputDevice) {
        DelayedSingleton<DeviceInfoService>::GetInstance()->HandleInputDeviceInfoCallback(inputDevice);
    });
    std::pair<int32_t, InputDeviceInfo> pair;
    if (result == GAME_CONTROLLER_SUCCESS) {
        taskConditionVar_.wait_for(lock, std::chrono::milliseconds(QUERY_DEVICE_TIMEOUT_MS),
                                   [this]() { return taskFinish_; });
        if (taskFinish_ && inputDevice_ != nullptr) {
            pair.first = GAME_CONTROLLER_SUCCESS;
            pair.second = BuildInputDeviceInfo();
        } else {
            HILOGE("[GameController]DeviceInfoService GetDevice timeout.");
            pair.first = GAME_ERR_TIMEOUT;
        }
    } else {
        HILOGE("[GameController]DeviceInfoService GetDevice failed. The error is  %{public}d",
               result);
        pair.first = GAME_ERR_FAIL;
    }
    return pair;
}

InputDeviceInfo DeviceInfoService::BuildInputDeviceInfo()
{
    InputDeviceInfo inputDeviceInfo;
    inputDeviceInfo.vendor = inputDevice_->GetVendor();
    inputDeviceInfo.product = inputDevice_->GetProduct();
    inputDeviceInfo.name = inputDevice_->GetName();
    inputDeviceInfo.uniq = inputDevice_->GetUniq();
    inputDeviceInfo.id = inputDevice_->GetId();
    inputDeviceInfo.version = inputDevice_->GetVersion();
    inputDeviceInfo.phys = inputDevice_->GetPhys();
    std::unordered_set<InputSourceTypeEnum> sources;
    for (const auto &item: g_inputSourceType) {
        if (static_cast<uint32_t>(inputDevice_->GetType()) & item.typeBit) {
            sources.insert(item.inputSourceTypeEnum);
        }
    }
    inputDeviceInfo.sourceTypeSet = sources;
    return inputDeviceInfo;
}

void DeviceInfoService::HandleInputDeviceInfoCallback(std::shared_ptr<InputDevice> inputDevice)
{
    inputDevice_ = inputDevice;
    taskFinish_ = true;
    taskConditionVar_.notify_all();
}

std::pair<int32_t, std::vector<InputDeviceInfo>> DeviceInfoService::GetAllDeviceInfos()
{
    std::pair<int32_t, std::vector<InputDeviceInfo>> pair;

    // Obtain the IDs of all online devices.
    std::pair<int32_t, std::vector<int32_t>> idsPair = GetAllDeviceIds();
    if (idsPair.first != GAME_CONTROLLER_SUCCESS || idsPair.second.empty()) {
        pair.first = idsPair.first;
        return pair;
    }

    // Queries device information by device ID.
    std::vector<InputDeviceInfo> infos;
    for (auto id: idsPair.second) {
        std::pair<int32_t, InputDeviceInfo> deviceInfoPair = GetInputDeviceInfo(id);
        if (deviceInfoPair.first != GAME_CONTROLLER_SUCCESS) {
            continue;
        }

        InputDeviceInfo inputDeviceInfo = deviceInfoPair.second;
        if (!GetUniqOnGetAllDeviceInfos(infos, inputDeviceInfo)) {
            continue;
        }
        HILOGI("[GameController]GetAllDeviceInfos InputDeviceInfo %{public}s",
               inputDeviceInfo.GetDeviceInfoDesc().c_str());
        infos.push_back(inputDeviceInfo);
    }
    pair.second = infos;
    return pair;
}

bool DeviceInfoService::GetUniqOnGetAllDeviceInfos(std::vector<InputDeviceInfo> &infos,
                                                   InputDeviceInfo &inputDeviceInfo)
{
    if (!inputDeviceInfo.UniqIsEmpty()) {
        return true;
    }

    // uniq is empty in the virtual device information
    if (!inputDeviceInfo.IsVirtualDeviceForExternalDevice()) {
        // The system's own virtual devices do not need to be notified to the user.
        HILOGW("[GameController]GetAllDeviceInfos discard system InputDeviceInfo %{public}s. "
               "it's system virtual device",
               inputDeviceInfo.GetDeviceInfoDesc().c_str());
        return false;
    }

    inputDeviceInfo.uniq = std::to_string(inputDeviceInfo.vendor) + "_"
        + std::to_string(inputDeviceInfo.product);
    HILOGI("[GameController] GetAllDeviceInfos: Embeded device. "
           "deviceId is %{public}d, vendor is %{public}d, product is %{public}d. "
           "Because uniq is empty,construct a new uniq. ",
           inputDeviceInfo.id, inputDeviceInfo.vendor, inputDeviceInfo.product);
    return true;
}

void DeviceInfoService::HandleAllDeviceIdsCallback(std::vector<int32_t> &deviceIds)
{
    deviceIds_ = deviceIds;
    taskFinish_ = true;
    taskConditionVar_.notify_all();
}

std::pair<int32_t, std::vector<int32_t>> DeviceInfoService::GetAllDeviceIds()
{
    std::unique_lock<std::mutex> lock(taskMutex_);
    deviceIds_.clear();
    taskFinish_ = false;
    int result = InputManager::GetInstance()->GetDeviceIds([&](std::vector<int32_t> &ids) {
        DelayedSingleton<DeviceInfoService>::GetInstance()->HandleAllDeviceIdsCallback(ids);
    });
    std::pair<int32_t, std::vector<int32_t>> pair;
    if (result == GAME_CONTROLLER_SUCCESS) {
        taskConditionVar_.wait_for(lock, std::chrono::milliseconds(QUERY_DEVICE_TIMEOUT_MS),
                                   [this]() { return taskFinish_; });
        if (taskFinish_) {
            pair.first = GAME_CONTROLLER_SUCCESS;
            pair.second = deviceIds_;
        } else {
            HILOGE("[GameController]DeviceInfoService GetDeviceIds timeout.");
            pair.first = GAME_ERR_TIMEOUT;
        }
    } else {
        HILOGE("[GameController]DeviceInfoService GetDeviceIds failed. The error is  %{public}d",
               result);
        pair.first = GAME_ERR_CALL_MULTI_INPUT_FAIL;
    }
    return pair;
}

std::pair<int32_t, int32_t> DeviceInfoService::GetKeyBoardType(int32_t id)
{
    std::unique_lock<std::mutex> lock(taskMutex_);
    keyboardType_ = 0;
    int result = InputManager::GetInstance()->GetKeyboardType(id, [](int32_t keyboardType) {
        DelayedSingleton<DeviceInfoService>::GetInstance()->HandleKeyBoardTypeCallback(keyboardType);
    });
    std::pair<int32_t, int32_t> pair;
    if (result == GAME_CONTROLLER_SUCCESS) {
        taskConditionVar_.wait_for(lock, std::chrono::milliseconds(QUERY_DEVICE_TIMEOUT_MS),
                                   [this]() { return taskFinish_; });
        if (taskFinish_) {
            pair.first = GAME_CONTROLLER_SUCCESS;
            pair.second = keyboardType_;
        } else {
            HILOGE("[GameController]DeviceInfoService GetDeviceIds timeout.");
            pair.first = GAME_ERR_TIMEOUT;
        }
    } else {
        HILOGE("[GameController]DeviceInfoService GetKeyBoardType failed. The error is  %{public}d",
               result);
        pair.first = GAME_ERR_CALL_MULTI_INPUT_FAIL;
    }
    return pair;
}

void DeviceInfoService::HandleKeyBoardTypeCallback(int32_t keyboardType)
{
    keyboardType_ = keyboardType;
    taskConditionVar_.notify_all();
}

}
}