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

#ifndef GAME_CONTROLLER_DEVICE_INFO_SERVICE_H
#define GAME_CONTROLLER_DEVICE_INFO_SERVICE_H

#include <memory>
#include <mutex>
#include <condition_variable>
#include <singleton.h>
#include <unordered_set>
#include "input_device.h"
#include "gamecontroller_client_model.h"
#include "gamecontroller_utils.h"

namespace OHOS {
namespace GameController {
using namespace OHOS::MMI;
struct InputSourceType {
    InputSourceTypeEnum inputSourceTypeEnum;
    uint32_t typeBit{0};
};

enum DEV_TAG {
    EVDEV_UDEV_TAG_KEYBOARD = 1 << 1,
    EVDEV_UDEV_TAG_MOUSE = 1 << 2,
    EVDEV_UDEV_TAG_TOUCHPAD = 1 << 3,
    EVDEV_UDEV_TAG_TOUCHSCREEN = 1 << 4,
    EVDEV_UDEV_TAG_JOYSTICK = 1 << 6,
    EVDEV_UDEV_TAG_TRACKBALL = 1 << 10
};

/**
 * input device information
 */
struct InputDeviceInfo {
    int32_t id;
    std::string uniq;
    std::string name;
    int32_t product;
    int32_t vendor;
    int32_t version;
    std::string phys;
    std::unordered_set<InputSourceTypeEnum> sourceTypeSet;

    /**
     * Obtains the device information description.
     * @return Device information description
     */
    std::string GetDeviceInfoDesc()
    {
        std::string tmp = "id:" + std::to_string(id);
        tmp.append(", uniq:" + StringUtils::AnonymizationUniq(uniq));
        tmp.append(", name:" + name);
        tmp.append(", product:" + std::to_string(product));
        tmp.append(", vendor:" + std::to_string(vendor));
        tmp.append(", version:" + std::to_string(version));
        tmp.append(", phys:" + phys);
        tmp.append(", sourceTypeSet:");
        for (auto sourceType: sourceTypeSet) {
            tmp.append(std::to_string(sourceType));
            tmp.append("|");
        }
        return tmp;
    }

    /**
    * Check whether uniq is empty.
    * @return The value true indicates that the value is null.
    */
    bool UniqIsEmpty()
    {
        return uniq.empty() || uniq == "null";
    }

    /**
     * whether it's the  the virtual device of the external device
     * @return true means it's the virtual device of the external device
     */
    bool IsVirtualDeviceForExternalDevice()
    {
        return UniqIsEmpty() && vendor != 0 && product != 0;
    }

    std::string GenerateUniq()
    {
        return std::to_string(vendor) + "_" + std::to_string(product) + "_" + std::to_string(id);
    }
};

/**
 * Querying device information from asynchronous to synchronous
 */
class DeviceInfoService : public DelayedSingleton<DeviceInfoService> {
DECLARE_DELAYED_SINGLETON(DeviceInfoService)

public:
    virtual std::pair<int32_t, std::vector<InputDeviceInfo>> GetAllDeviceInfos();

    /**
     * Querying Device Information
     * @param deviceId Device ID.
     * @return first indicates the query result, and second indicates the device information.
     */
    virtual std::pair<int32_t, InputDeviceInfo> GetInputDeviceInfo(int32_t deviceId);

    /**
     * Get the keyboard type
     * @return first indicates the query result, and second indicates the keyboard type.
     */
    virtual std::pair<int32_t, int32_t> GetKeyBoardType(int32_t id);

    /**
     * Setting the current device information
     * @param inputDevice Device Information
     */
    void HandleInputDeviceInfoCallback(std::shared_ptr<InputDevice> inputDevice);

    /**
     * Callback for querying all device IDs
     * @param deviceIds All device IDs
     */
    void HandleAllDeviceIdsCallback(std::vector<int32_t> &deviceIds);

    /**
     * Callback for querying all device IDs
     * @param deviceIds All device IDs
     */
    void HandleKeyBoardTypeCallback(int32_t keyboardType);

private:
    /**
     * Queries all device IDs.
     * @return first indicates the query result, and second indicates the IDs of all devices.
     */
    std::pair<int32_t, std::vector<int32_t>> GetAllDeviceIds();

    InputDeviceInfo BuildInputDeviceInfo();

    /**
     * if uniq is empty, query uniq from infos by product and vendor
     * @param infos Device information list with uniq
     * @param inputDeviceInfo deviceInfo from multi-input
     * @return true means uniq is not empty
     */
    bool GetUniqOnGetAllDeviceInfos(std::vector<InputDeviceInfo> &infos, InputDeviceInfo &inputDeviceInfo);

private:
    /**
     * Lock for Asynchronous to Synchronous
     */
    std::mutex taskMutex_;

    /**
     * Task Check Conditions
     */
    std::condition_variable taskConditionVar_;

    /**
     * Whether the asynchronous-to-synchronous task is complete
     */
    bool taskFinish_{false};

    /**
     * Device query result
     */
    std::shared_ptr<InputDevice> inputDevice_;

    /**
     * Query result of all device IDs
     */
    std::vector<int32_t> deviceIds_;

    int32_t keyboardType_{0};
};
}
}
#endif //GAME_CONTROLLER_DEVICE_INFO_SERVICE_H