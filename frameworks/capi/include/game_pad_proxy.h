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

#ifndef GAME_CONTROLLER_FRAMEWORK_GAME_PAD_PROXY_H
#define GAME_CONTROLLER_FRAMEWORK_GAME_PAD_PROXY_H

#include <singleton.h>
#include "game_pad_event.h"
#include "game_controller_type.h"
#include "gamecontroller_client_model.h"

namespace OHOS {
namespace GameController {
class GamePadButtonEventCallback : public GamePadButtonCallbackBase {
public:
    GamePadButtonEventCallback() = default;

    ~GamePadButtonEventCallback() = default;

    int32_t OnButtonEventCallback(const GamePadButtonEvent &event) override;

    void SetCallback(GamePad_ButtonInputMonitorCallback callback);

private:
    GamePad_ButtonInputMonitorCallback callback_ = nullptr;
};

class GamePadAxisEventCallback : public GamePadAxisCallbackBase {
public:
    GamePadAxisEventCallback() = default;

    ~GamePadAxisEventCallback() = default;

    int32_t OnAxisEventCallback(const GamePadAxisEvent &event) override;

    void SetCallback(GamePad_AxisInputMonitorCallback callback);

private:
    GamePad_AxisInputMonitorCallback callback_ = nullptr;
};

class GamePadProxy : public Singleton<GamePadProxy> {
DECLARE_SINGLETON(GamePadProxy);
public:
    GameController_ErrorCode LeftShoulder_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode LeftShoulder_UnRegisterButtonInputMonitor();

    GameController_ErrorCode RightShoulder_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode RightShoulder_UnRegisterButtonInputMonitor();

    GameController_ErrorCode LeftTrigger_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode LeftTrigger_UnRegisterButtonInputMonitor();

    GameController_ErrorCode LeftTrigger_RegisterAxisInputMonitor(
        GamePad_AxisInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode LeftTrigger_UnRegisterAxisInputMonitor();

    GameController_ErrorCode RightTrigger_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode RightTrigger_UnRegisterButtonInputMonitor();

    GameController_ErrorCode RightTrigger_RegisterAxisInputMonitor(
        GamePad_AxisInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode RightTrigger_UnRegisterAxisInputMonitor();

    GameController_ErrorCode ButtonMenu_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode ButtonMenu_UnRegisterButtonInputMonitor();

    GameController_ErrorCode ButtonHome_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode ButtonHome_UnRegisterButtonInputMonitor();

    GameController_ErrorCode ButtonA_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode ButtonA_UnRegisterButtonInputMonitor();

    GameController_ErrorCode ButtonB_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode ButtonB_UnRegisterButtonInputMonitor();

    GameController_ErrorCode ButtonX_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode ButtonX_UnRegisterButtonInputMonitor();

    GameController_ErrorCode ButtonY_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode ButtonY_UnRegisterButtonInputMonitor();

    GameController_ErrorCode ButtonC_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode ButtonC_UnRegisterButtonInputMonitor();

    GameController_ErrorCode Dpad_LeftButton_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode Dpad_LeftButton_UnRegisterButtonInputMonitor();

    GameController_ErrorCode Dpad_RightButton_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode Dpad_RightButton_UnRegisterButtonInputMonitor();

    GameController_ErrorCode Dpad_UpButton_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode Dpad_UpButton_UnRegisterButtonInputMonitor();

    GameController_ErrorCode Dpad_DownButton_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode Dpad_DownButton_UnRegisterButtonInputMonitor();

    GameController_ErrorCode Dpad_RegisterAxisInputMonitor(
        GamePad_AxisInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode Dpad_UnRegisterAxisInputMonitor();

    GameController_ErrorCode LeftThumbstick_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode LeftThumbstick_UnRegisterButtonInputMonitor();

    GameController_ErrorCode LeftThumbstick_RegisterAxisInputMonitor(
        GamePad_AxisInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode LeftThumbstick_UnRegisterAxisInputMonitor();

    GameController_ErrorCode RightThumbstick_RegisterButtonInputMonitor(
        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode RightThumbstick_UnRegisterButtonInputMonitor();

    GameController_ErrorCode RightThumbstick_RegisterAxisInputMonitor(
        GamePad_AxisInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode RightThumbstick_UnRegisterAxisInputMonitor();

private:
    GameController_ErrorCode RegisterButtonInputMonitor(GamePadButtonTypeEnum buttonTypeEnum,
                                                        GamePad_ButtonInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode UnRegisterButtonInputMonitor(GamePadButtonTypeEnum buttonTypeEnum);

    GameController_ErrorCode RegisterAxisInputMonitor(GamePadAxisSourceTypeEnum axisTypeEnum,
                                                      GamePad_AxisInputMonitorCallback inputMonitorCallback);

    GameController_ErrorCode UnRegisterAxisInputMonitor(GamePadAxisSourceTypeEnum axisTypeEnum);
};
}
}

#endif //GAME_CONTROLLER_FRAMEWORK_GAME_PAD_PROXY_H
