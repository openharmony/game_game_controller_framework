/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GAME_CONTROLLER_FRAMEWORK_GAME_DEVICE_EVENT_PROXY_H
#define GAME_CONTROLLER_FRAMEWORK_GAME_DEVICE_EVENT_PROXY_H

#include <singleton.h>
#include "game_controller_type.h"
#include "game_device_event.h"

namespace OHOS {
namespace GameController {
class GameDeviceEventProxy : public Singleton<GameDeviceEventProxy> {
DECLARE_SINGLETON(GameDeviceEventProxy);
public:
    GameController_ErrorCode GetChangedType(const struct GameDevice_DeviceEvent* deviceEvent,
                                            GameDevice_StatusChangedType* statusChangedType);

    GameController_ErrorCode GetDeviceInfo(const struct GameDevice_DeviceEvent* deviceEvent,
                                           GameDevice_DeviceInfo** deviceInfo);

    GameController_ErrorCode DestroyDeviceInfo(GameDevice_DeviceInfo** deviceInfo);

    GameController_ErrorCode GetDeviceIdFromDeviceInfo(const struct GameDevice_DeviceInfo* deviceInfo,
                                                       char** deviceId);

    GameController_ErrorCode GetNameFromDeviceInfo(const struct GameDevice_DeviceInfo* deviceInfo,
                                                   char** name);

    GameController_ErrorCode GetProductFromDeviceInfo(const struct GameDevice_DeviceInfo* deviceInfo,
                                                      int32_t* product);

    GameController_ErrorCode GetVersionFromDeviceInfo(const struct GameDevice_DeviceInfo* deviceInfo,
                                                      int32_t* version);

    GameController_ErrorCode GetPhysFromDeviceInfo(const struct GameDevice_DeviceInfo* deviceInfo,
                                                   char** phys);

    GameController_ErrorCode GetDeviceTypeFromDeviceInfo(const struct GameDevice_DeviceInfo* deviceInfo,
                                                         GameDevice_DeviceType* deviceType);
};
}
}

#endif //GAME_CONTROLLER_FRAMEWORK_GAME_DEVICE_EVENT_PROXY_H
