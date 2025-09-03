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

#ifndef GAME_CONTROLLER_DEVICE_MANAGER_H
#define GAME_CONTROLLER_DEVICE_MANAGER_H

#include <vector>
#include <map>
#include <unordered_map>
#include <cstdint>
#include "singleton.h"
#include "gamecontroller_client_model.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace GameController {
struct IdentifyDeviceInfo {
    /**
     * Product Code
     */
    int32_t productId = -1;

    /**
     * vendor code
     */
    int32_t vendor = -1;

    /**
     * Device Type
     */
    int32_t deviceType = -1;

    /**
     * Device Name
     */
    std::string namePrefix = "";

    IdentifyDeviceInfo();

    IdentifyDeviceInfo(IdentifiedDeviceInfo info)
    {
        this->productId = info.product;
        this->vendor = info.vendor;
        this->deviceType = static_cast<int32_t>(info.deviceType);
        this->namePrefix = info.name;
    }

    explicit IdentifyDeviceInfo(const nlohmann::json &jsonObj);

    /**
     * Check whether the device matches the device to be identified.
     *
     * @param device
     * @return The value true indicates that the device is successfully identified.
     */
    bool IsMatched(const DeviceInfo &device) const;

    nlohmann::json ConvertToJson() const;
};

class DeviceManager : public std::enable_shared_from_this<DeviceManager> {
DECLARE_DELAYED_SINGLETON(DeviceManager)

public:
    /**
     * Identify the device and return the identified device.
     *
     * @param deviceInfos Information about the device to be identified
     * @param identifyResult Identification result
     * @return 0: success. Other values fail.
     */
    int32_t DeviceIdentify(const std::vector<DeviceInfo> &deviceInfos, std::vector<DeviceInfo> &identifyResult);

    /**
     * Synchronize identified devices
     * @param deviceInfos identified devices
     * @return 0: success. Other values fail.
     */
    int32_t SyncIdentifiedDeviceInfos(const std::vector<IdentifiedDeviceInfo> &deviceInfos);

private:
    void LoadDeviceCacheFile();

    void LoadFromJson(const nlohmann::json &config);

    void BuildDeviceInfoWithType(const DeviceInfo &src, std::vector<DeviceInfo> &result, DeviceTypeEnum type);

    int32_t SaveToConfigFile(nlohmann::json &jsonContent, std::vector<IdentifyDeviceInfo> &devices);

private:
    /**
     * Identified Devices
     */
    std::vector<IdentifyDeviceInfo> identifiedDevices_;

    std::mutex mMutex_;
};
}
}
#endif //GAME_CONTROLLER_DEVICE_MANAGER_H
