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
#ifndef GAME_CONTROLLER_INPUT_DEVICE_LISTENER_H
#define GAME_CONTROLLER_INPUT_DEVICE_LISTENER_H

#include <cstdint>
#include <string>
#include <i_input_device_listener.h>
#include "gamecontroller_constants.h"
#include "gamecontroller_client_model.h"

namespace OHOS {
namespace GameController {
/**
 * Device status callback
 */
class InputDeviceListener : public OHOS::MMI::IInputDeviceListener {
public:
    InputDeviceListener();

    ~InputDeviceListener();

    void OnDeviceAdded(int32_t deviceId, const std::string &type) override;

    void OnDeviceRemoved(int32_t deviceId, const std::string &type) override;

private:
    void HandleEvent(int32_t deviceId, DeviceChangeType deviceChangeType);
};
}
}
#endif //GAME_CONTROLLER_INPUT_DEVICE_LISTENER_H
