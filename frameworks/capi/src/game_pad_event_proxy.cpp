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

#include "game_pad_event_proxy.h"
#include "gamecontroller_client_model.h"
#include "gamecontroller_log.h"
#include "gamecontroller_utils.h"

namespace OHOS {
namespace GameController {
GamePadEventProxy::GamePadEventProxy()
{
}

GamePadEventProxy::~GamePadEventProxy()
{
}

GameController_ErrorCode GamePadEventProxy::GetDeviceIdFromButtonEvent(
    const struct GamePad_ButtonEvent* buttonEvent, char** deviceId)
{
    if (buttonEvent == nullptr || deviceId == nullptr) {
        HILOGE("[CAPI][GetDeviceIdFromButtonEvent] buttonEvent or deviceId is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }

    int32_t ret = StringUtils::ConvertToCharPtrArray(((GamePadButtonEvent*)buttonEvent)->uniq, deviceId);
    if (ret == GAME_CONTROLLER_SUCCESS) {
        return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
    }
    return GameController_ErrorCode::GAME_CONTROLLER_NO_MEMORY;
}

GameController_ErrorCode GamePadEventProxy::GetButtonActionFromButtonEvent(
    const struct GamePad_ButtonEvent* buttonEvent,
    GamePad_Button_ActionType* actionType)
{
    if (buttonEvent == nullptr) {
        HILOGE("[CAPI][GetButtonActionFromButtonEvent]buttonEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    int32_t keyAction = ((GamePadButtonEvent*)buttonEvent)->keyAction;
    *actionType = static_cast<GamePad_Button_ActionType>(keyAction);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetButtonCodeFromButtonEvent(
    const struct GamePad_ButtonEvent* buttonEvent, int32_t* code)
{
    if (buttonEvent == nullptr) {
        HILOGE("[CAPI][GetButtonCodeFromButtonEvent]buttonEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *code = ((GamePadButtonEvent*)buttonEvent)->keyCode;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetButtonCodeNameFromButtonEvent(
    const struct GamePad_ButtonEvent* buttonEvent, char** codeName)
{
    if (buttonEvent == nullptr || codeName == nullptr) {
        HILOGE("[CAPI][GetButtonCodeNameFromButtonEvent]buttonEvent or codeName is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }

    int32_t ret = StringUtils::ConvertToCharPtrArray(((GamePadButtonEvent*)buttonEvent)->keyCodeName, codeName);
    if (ret == GAME_CONTROLLER_SUCCESS) {
        return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
    }
    return GameController_ErrorCode::GAME_CONTROLLER_NO_MEMORY;
}

GameController_ErrorCode GamePadEventProxy::GetCountFromPressedButtons(
    const struct GamePad_ButtonEvent* buttonEvent, int32_t* count)
{
    if (buttonEvent == nullptr) {
        HILOGE("[CAPI][GetCountFromPressedButtons]buttonEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }

    *count = static_cast<int32_t>(((GamePadButtonEvent*)buttonEvent)->keys.size());
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetButtonInfoFromPressedButtons(
    const struct GamePad_ButtonEvent* buttonEvent, const int32_t index,
    GamePad_PressedButton** pressedButton)
{
    if (buttonEvent == nullptr) {
        HILOGE("[CAPI][GetButtonInfoFromPressedButtons]buttonEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }

    GamePadButtonEvent* event = ((GamePadButtonEvent*)buttonEvent);
    int32_t count = static_cast<int32_t>(event->keys.size());
    if (index >= count || index < 0) {
        HILOGE("[CAPI][GetButtonInfoFromPressedButtons]index is over range");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    KeyInfo pressKey = event->keys.at(index);
    KeyInfo* keyInfo = new KeyInfo();
    keyInfo->keyCodeName = pressKey.keyCodeName;
    keyInfo->keyCode = pressKey.keyCode;
    *pressedButton = (GamePad_PressedButton*)keyInfo;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::DestroyPressedButton(GamePad_PressedButton** pressedButton)
{
    if (pressedButton == nullptr) {
        HILOGE("[CAPI][DestroyPressedButton]pressedButton is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    delete ((KeyInfo*)*pressedButton);
    *pressedButton = nullptr;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetButtonCodeFromPressedButton(
    const struct GamePad_PressedButton* pressedButton, int32_t* code)
{
    if (pressedButton == nullptr) {
        HILOGE("[CAPI][GetButtonCodeFromPressedButton]pressedButton is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *code = ((KeyInfo*)pressedButton)->keyCode;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetButtonCodeNameFromPressedButton(
    const struct GamePad_PressedButton* pressedButton,
    char** codeName)
{
    if (pressedButton == nullptr || codeName == nullptr) {
        HILOGE("[CAPI][GetButtonCodeNameFromPressedButton]pressedButton or codeName is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }

    int32_t ret = StringUtils::ConvertToCharPtrArray(((KeyInfo*)pressedButton)->keyCodeName, codeName);
    if (ret == GAME_CONTROLLER_SUCCESS) {
        return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
    }
    return GameController_ErrorCode::GAME_CONTROLLER_NO_MEMORY;
}

GameController_ErrorCode GamePadEventProxy::GetActionTimeFromButtonEvent(
    const struct GamePad_ButtonEvent* buttonEvent, int64_t* actionTime)
{
    if (buttonEvent == nullptr) {
        HILOGE("[CAPI][GetActionTimeFromButtonEvent]buttonEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *actionTime = ((GamePadButtonEvent*)buttonEvent)->actionTime;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetDeviceIdFromAxisEvent(
    const struct GamePad_AxisEvent* axisEvent, char** deviceId)
{
    if (axisEvent == nullptr || deviceId == nullptr) {
        HILOGE("[CAPI][DestroyAxisEvent]axisEvent or deviceId is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }

    int32_t ret = StringUtils::ConvertToCharPtrArray(((GamePadAxisEvent*)axisEvent)->uniq, deviceId);
    if (ret == GAME_CONTROLLER_SUCCESS) {
        return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
    }
    return GameController_ErrorCode::GAME_CONTROLLER_NO_MEMORY;
}

GameController_ErrorCode GamePadEventProxy::GetAxisSourceTypeFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                                           GamePad_AxisSourceType* axisSourceType)
{
    if (axisEvent == nullptr) {
        HILOGE("[CAPI][GetAxisSourceTypeFromAxisEvent]axisEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    int32_t type = ((GamePadAxisEvent*)axisEvent)->axisSourceType;
    *axisSourceType = static_cast<GamePad_AxisSourceType>(type);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetXAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                                       double* axisValue)
{
    if (axisEvent == nullptr) {
        HILOGE("[CAPI][GetXAxisValueFromAxisEvent]axisEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *axisValue = ((GamePadAxisEvent*)axisEvent)->xValue;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetYAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                                       double* axisValue)
{
    if (axisEvent == nullptr) {
        HILOGE("[CAPI][GetYAxisValueFromAxisEvent]axisEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *axisValue = ((GamePadAxisEvent*)axisEvent)->yValue;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetZAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                                       double* axisValue)
{
    if (axisEvent == nullptr) {
        HILOGE("[CAPI][GetZAxisValueFromAxisEvent]axisEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *axisValue = ((GamePadAxisEvent*)axisEvent)->zValue;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetRZAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                                        double* axisValue)
{
    if (axisEvent == nullptr) {
        HILOGE("[CAPI][GetRZAxisValueFromAxisEvent]axisEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *axisValue = ((GamePadAxisEvent*)axisEvent)->rzValue;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetHatXAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                                          double* axisValue)
{
    if (axisEvent == nullptr) {
        HILOGE("[CAPI][GetHatXAxisValueFromAxisEvent]axisEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *axisValue = ((GamePadAxisEvent*)axisEvent)->hatxValue;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetHatYAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                                          double* axisValue)
{
    if (axisEvent == nullptr) {
        HILOGE("[CAPI][GetHatYAxisValueFromAxisEvent]axisEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *axisValue = ((GamePadAxisEvent*)axisEvent)->hatyValue;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetGasAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                                         double* axisValue)
{
    if (axisEvent == nullptr) {
        HILOGE("[CAPI][GetGasAxisValueFromAxisEvent]axisEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *axisValue = ((GamePadAxisEvent*)axisEvent)->gasValue;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetBrakeAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                                           double* axisValue)
{
    if (axisEvent == nullptr) {
        HILOGE("[CAPI][GetBrakeAxisValueFromAxisEvent]axisEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *axisValue = ((GamePadAxisEvent*)axisEvent)->brakeValue;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadEventProxy::GetActionTimeFromAxisEvent(
    const struct GamePad_AxisEvent* axisEvent, int64_t* actionTime)
{
    if (axisEvent == nullptr) {
        HILOGE("[CAPI][GetActionTimeFromAxisEvent]axisEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *actionTime = ((GamePadAxisEvent*)axisEvent)->actionTime;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

}
}