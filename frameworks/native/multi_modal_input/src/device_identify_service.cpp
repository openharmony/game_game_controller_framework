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

#include "device_identify_service.h"
#include "gamecontroller_server_client.h"
#include "gamecontroller_errors.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
DeviceIdentifyService::DeviceIdentifyService()
{
}

DeviceIdentifyService::~DeviceIdentifyService()
{
}

std::vector<DeviceInfo> DeviceIdentifyService::IdentifyDeviceType(const std::vector<DeviceInfo> &deviceInfos)
{
    std::vector<DeviceInfo> result;
    if (deviceInfos.size() == 0) {
        return result;
    }
    std::vector<DeviceInfo> req;
    int32_t ret = DelayedSingleton<GameControllerServerClient>::GetInstance()->IdentifyDevice(deviceInfos, result);
    if (ret == GAME_CONTROLLER_SUCCESS) {
        for (const auto &deviceInfo: result) {
            identifyResultMap_[deviceInfo.uniq] = deviceInfo.deviceType;
        }
        return result;
    }
    HILOGE("[GameController]IdentifyDeviceType failed. the error is %{public}d", ret);

    // When the SA is abnormal, the local query is guaranteed.
    for (auto deviceInfo: deviceInfos) {
        deviceInfo.deviceType = IdentifyDeviceTypeByLocal(deviceInfo);
        result.push_back(deviceInfo);
    }
    return result;
}

DeviceTypeEnum DeviceIdentifyService::IdentifyDeviceTypeByLocal(const DeviceInfo &deviceInfo)
{
    if (identifyResultMap_.find(deviceInfo.uniq) != identifyResultMap_.end()) {
        return identifyResultMap_[deviceInfo.uniq];
    }
    return DeviceTypeEnum::UNKNOWN;
}

}
}