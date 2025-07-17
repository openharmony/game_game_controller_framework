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


#ifndef GAME_CONTROLLER_FRAMEWORK_GAME_DEVICE_CLIENT_H
#define GAME_CONTROLLER_FRAMEWORK_GAME_DEVICE_CLIENT_H

#include <memory>
#include "gamecontroller_client_model.h"

namespace OHOS {
namespace GameController {
/**
 * Inner APIs related to devices
 */
class GameDeviceClient {
public:
    /**
     * Registering a Device Callback Event
     * @param apiTypeEnum API Source
     * @param callback Callback Method
     */
    static void RegisterGameDeviceEventCallback(const ApiTypeEnum apiTypeEnum,
                                                const std::shared_ptr<DeviceEventCallbackBase> &callback);

    /**
    * Device unregistration callback event
    * @param apiTypeEnum API Source
    */
    static void UnRegisterGameDeviceEventCallback(const ApiTypeEnum apiTypeEnum);

    /**
     * Get All Devices
     * @return first indicates the query result, and second indicates the information of all devices.
     */
    static std::pair<int32_t, std::vector<DeviceInfo>> GetAllDeviceInfos();
};
}
}

#endif //GAME_CONTROLLER_FRAMEWORK_GAME_DEVICE_CLIENT_H
