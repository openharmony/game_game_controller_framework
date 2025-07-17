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


#include "game_pad_event.h"
#include "game_pad_event_proxy.h"

GameController_ErrorCode OH_GamePad_ButtonEvent_GetDeviceId(const struct GamePad_ButtonEvent* buttonEvent,
                                                            char** deviceId)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetDeviceIdFromButtonEvent(buttonEvent, deviceId);
}

GameController_ErrorCode OH_GamePad_ButtonEvent_GetButtonAction(const struct GamePad_ButtonEvent* buttonEvent,
                                                                GamePad_Button_ActionType* actionType)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetButtonActionFromButtonEvent(buttonEvent,
                                                                                                 actionType);
}

GameController_ErrorCode OH_GamePad_ButtonEvent_GetButtonCode(const struct GamePad_ButtonEvent* buttonEvent,
                                                              int32_t* code)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetButtonCodeFromButtonEvent(buttonEvent, code);
}

GameController_ErrorCode OH_GamePad_ButtonEvent_GetButtonCodeName(const struct GamePad_ButtonEvent* buttonEvent,
                                                                  char** codeName)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetButtonCodeNameFromButtonEvent(buttonEvent,
                                                                                                   codeName);
}

GameController_ErrorCode OH_GamePad_PressedButtons_GetCount(const struct GamePad_ButtonEvent* buttonEvent,
                                                            int32_t* count)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetCountFromPressedButtons(buttonEvent, count);
}

GameController_ErrorCode OH_GamePad_PressedButtons_GetButtonInfo(const struct GamePad_ButtonEvent* buttonEvent,
                                                                 const int32_t index,
                                                                 GamePad_PressedButton** pressedButton)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetButtonInfoFromPressedButtons(buttonEvent, index,
                                                                                                  pressedButton);
}

GameController_ErrorCode OH_GamePad_DestroyPressedButton(GamePad_PressedButton** pressedButton)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().DestroyPressedButton(pressedButton);
}

GameController_ErrorCode OH_GamePad_PressedButton_GetButtonCode(const struct GamePad_PressedButton* pressedButton,
                                                                int32_t* code)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetButtonCodeFromPressedButton(pressedButton, code);
}

GameController_ErrorCode OH_GamePad_PressedButton_GetButtonCodeName(const struct GamePad_PressedButton* pressedButton,
                                                                    char** codeName)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetButtonCodeNameFromPressedButton(pressedButton,
                                                                                                     codeName);
}

GameController_ErrorCode OH_GamePad_ButtonEvent_GetActionTime(const struct GamePad_ButtonEvent* buttonEvent,
                                                              int64_t* actionTime)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetActionTimeFromButtonEvent(buttonEvent, actionTime);
}

GameController_ErrorCode OH_GamePad_AxisEvent_GetDeviceId(const struct GamePad_AxisEvent* axisEvent, char** deviceId)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetDeviceIdFromAxisEvent(axisEvent, deviceId);
}

GameController_ErrorCode OH_GamePad_AxisEvent_GetAxisSourceType(const struct GamePad_AxisEvent* axisEvent,
                                                                GamePad_AxisSourceType* axisSourceType)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetAxisSourceTypeFromAxisEvent(axisEvent,
                                                                                                 axisSourceType);
}

GameController_ErrorCode OH_GamePad_AxisEvent_GetXAxisValue(const struct GamePad_AxisEvent* axisEvent,
                                                            double* axisValue)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetXAxisValueFromAxisEvent(axisEvent, axisValue);
}

GameController_ErrorCode OH_GamePad_AxisEvent_GetYAxisValue(const struct GamePad_AxisEvent* axisEvent,
                                                            double* axisValue)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetYAxisValueFromAxisEvent(axisEvent, axisValue);
}

GameController_ErrorCode OH_GamePad_AxisEvent_GetZAxisValue(const struct GamePad_AxisEvent* axisEvent,
                                                            double* axisValue)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetZAxisValueFromAxisEvent(axisEvent, axisValue);
}

GameController_ErrorCode OH_GamePad_AxisEvent_GetRZAxisValue(const struct GamePad_AxisEvent* axisEvent,
                                                             double* axisValue)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetRZAxisValueFromAxisEvent(axisEvent, axisValue);
}

GameController_ErrorCode OH_GamePad_AxisEvent_GetHatXAxisValue(const struct GamePad_AxisEvent* axisEvent,
                                                               double* axisValue)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetHatXAxisValueFromAxisEvent(axisEvent, axisValue);
}

GameController_ErrorCode OH_GamePad_AxisEvent_GetHatYAxisValue(const struct GamePad_AxisEvent* axisEvent,
                                                               double* axisValue)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetHatYAxisValueFromAxisEvent(axisEvent, axisValue);
}

GameController_ErrorCode OH_GamePad_AxisEvent_GetBrakeAxisValue(const struct GamePad_AxisEvent* axisEvent,
                                                                double* axisValue)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetBrakeAxisValueFromAxisEvent(axisEvent, axisValue);
}

GameController_ErrorCode OH_GamePad_AxisEvent_GetGasAxisValue(const struct GamePad_AxisEvent* axisEvent,
                                                              double* axisValue)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetGasAxisValueFromAxisEvent(axisEvent, axisValue);
}

GameController_ErrorCode OH_GamePad_AxisEvent_GetActionTime(const struct GamePad_AxisEvent* axisEvent,
                                                            int64_t* actionTime)
{
    return OHOS::GameController::GamePadEventProxy::GetInstance().GetActionTimeFromAxisEvent(axisEvent, actionTime);
}

