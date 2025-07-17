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


#include "game_pad.h"
#include "game_pad_proxy.h"

GameController_ErrorCode OH_GamePad_LeftShoulder_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().LeftShoulder_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_LeftShoulder_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().LeftShoulder_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_RightShoulder_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().RightShoulder_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_RightShoulder_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().RightShoulder_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_LeftTrigger_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().LeftTrigger_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_LeftTrigger_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().LeftTrigger_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_LeftTrigger_RegisterAxisInputMonitor(
    GamePad_AxisInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().LeftTrigger_RegisterAxisInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_LeftTrigger_UnregisterAxisInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().LeftTrigger_UnRegisterAxisInputMonitor();
}

GameController_ErrorCode OH_GamePad_RightTrigger_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().RightTrigger_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_RightTrigger_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().RightTrigger_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_RightTrigger_RegisterAxisInputMonitor(
    GamePad_AxisInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().RightTrigger_RegisterAxisInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_RightTrigger_UnregisterAxisInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().RightTrigger_UnRegisterAxisInputMonitor();
}

GameController_ErrorCode OH_GamePad_ButtonMenu_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonMenu_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_ButtonMenu_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonMenu_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_ButtonHome_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonHome_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_ButtonHome_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonHome_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_ButtonA_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonA_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_ButtonA_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonA_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_ButtonB_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonB_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_ButtonB_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonB_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_ButtonX_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonX_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_ButtonX_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonX_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_ButtonY_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonY_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_ButtonY_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonY_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_ButtonC_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonC_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_ButtonC_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().ButtonC_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_Dpad_LeftButton_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().Dpad_LeftButton_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_Dpad_LeftButton_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().Dpad_LeftButton_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_Dpad_RightButton_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().Dpad_RightButton_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_Dpad_RightButton_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().Dpad_RightButton_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_Dpad_UpButton_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().Dpad_UpButton_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_Dpad_UpButton_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().Dpad_UpButton_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_Dpad_DownButton_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().Dpad_DownButton_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_Dpad_DownButton_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().Dpad_DownButton_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_Dpad_RegisterAxisInputMonitor(
    GamePad_AxisInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().Dpad_RegisterAxisInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_Dpad_UnregisterAxisInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().Dpad_UnRegisterAxisInputMonitor();
}

GameController_ErrorCode OH_GamePad_LeftThumbstick_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().LeftThumbstick_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_LeftThumbstick_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().LeftThumbstick_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_LeftThumbstick_RegisterAxisInputMonitor(
    GamePad_AxisInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().LeftThumbstick_RegisterAxisInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_LeftThumbstick_UnregisterAxisInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().LeftThumbstick_UnRegisterAxisInputMonitor();
}

GameController_ErrorCode OH_GamePad_RightThumbstick_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().RightThumbstick_RegisterButtonInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_RightThumbstick_UnregisterButtonInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().RightThumbstick_UnRegisterButtonInputMonitor();
}

GameController_ErrorCode OH_GamePad_RightThumbstick_RegisterAxisInputMonitor(
    GamePad_AxisInputMonitorCallback inputMonitorCallback)
{
    return OHOS::GameController::GamePadProxy::GetInstance().RightThumbstick_RegisterAxisInputMonitor(
        inputMonitorCallback);
}

GameController_ErrorCode OH_GamePad_RightThumbstick_UnregisterAxisInputMonitor(void)
{
    return OHOS::GameController::GamePadProxy::GetInstance().RightThumbstick_UnRegisterAxisInputMonitor();
}

