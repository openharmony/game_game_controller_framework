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
#include "device_info_service.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t ALPHABETIC_KEYBOARD_TYPE = 2; // Full keyboard
}

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
    std::unordered_map<std::string, DeviceInfo> deviceMap;
    for (const auto &deviceInfo: deviceInfos) {
        deviceMap[deviceInfo.uniq] = deviceInfo;
    }
    std::vector<DeviceInfo> req;
    int32_t ret = DelayedSingleton<GameControllerServerClient>::GetInstance()->IdentifyDevice(deviceInfos, result);
    if (ret == GAME_CONTROLLER_SUCCESS) {
        for (auto &deviceInfo: result) {
            if (deviceMap.count(deviceInfo.uniq) != 0) {
                deviceInfo.idSourceTypeMap = deviceMap[deviceInfo.uniq].idSourceTypeMap;
            }

            if (deviceInfo.deviceType != DeviceTypeEnum::UNKNOWN) {
                continue;
            }

            /**
             * When the device type is unknown and keyboard exists in the sourceType,
             * obtain the keyboard type. If the keyboard type is ALPHABETIC_KEYBOARD,
             * it's full keyboard. hasFullKeyBoard is used for key_to_touch
             */
            deviceInfo.hasFullKeyBoard = HasFullKeyboard(deviceInfo);
        }
        return result;
    }
    HILOGE("IdentifyDeviceType failed. the error is %{public}d", ret);
    for (const auto &deviceInfo: deviceInfos) {
        result.push_back(deviceInfo);
    }
    return result;
}

bool DeviceIdentifyService::HasFullKeyboard(const DeviceInfo &deviceInfo)
{
    if (deviceInfo.sourceTypeSet.count(InputSourceTypeEnum::KEYBOARD) == 0) {
        return false;
    }
    std::pair<int32_t, int32_t> result;
    for (const auto &pair: deviceInfo.idSourceTypeMap) {
        if (pair.second.count(InputSourceTypeEnum::KEYBOARD) == 0) {
            continue;
        }
        result = DelayedSingleton<DeviceInfoService>::GetInstance()->GetKeyBoardType(pair.first);
        if (result.first != GAME_CONTROLLER_SUCCESS) {
            continue;
        }
        HILOGI("The keyboard type of the deviceId[%{public}d] is [%{public}d]", pair.first, result.second);
        if (result.second == ALPHABETIC_KEYBOARD_TYPE) {
            return true;
        }
    }
    return false;
}

}
}