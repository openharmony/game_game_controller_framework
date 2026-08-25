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

#include "game_pad_proxy.h"
#include "gamecontroller_log.h"
#include "gamecontroller_errors.h"
#include "input_event_client.h"

namespace OHOS {
namespace GameController {
GamePadProxy::GamePadProxy()
{
}

GamePadProxy::~GamePadProxy()
{
}

GameController_ErrorCode GamePadProxy::LeftShoulder_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::LeftShoulder, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::LeftShoulder_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::LeftShoulder);
}

GameController_ErrorCode GamePadProxy::RightShoulder_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::RightShoulder, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::RightShoulder_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::RightShoulder);
}

GameController_ErrorCode GamePadProxy::LeftTrigger_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::LeftTrigger, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::LeftTrigger_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::LeftTrigger);
}

GameController_ErrorCode GamePadProxy::LeftTrigger_RegisterAxisInputMonitor(
    GamePad_AxisInputMonitorCallback inputMonitorCallback)
{
    return RegisterAxisInputMonitor(GamePadAxisSourceTypeEnum::LeftTriggerAxis, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::LeftTrigger_UnRegisterAxisInputMonitor()
{
    return UnRegisterAxisInputMonitor(GamePadAxisSourceTypeEnum::LeftTriggerAxis);
}

GameController_ErrorCode GamePadProxy::RightTrigger_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::RightTrigger, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::RightTrigger_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::RightTrigger);
}

GameController_ErrorCode GamePadProxy::RightTrigger_RegisterAxisInputMonitor(
    GamePad_AxisInputMonitorCallback inputMonitorCallback)
{
    return RegisterAxisInputMonitor(GamePadAxisSourceTypeEnum::RightTriggerAxis, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::RightTrigger_UnRegisterAxisInputMonitor()
{
    return UnRegisterAxisInputMonitor(GamePadAxisSourceTypeEnum::RightTriggerAxis);
}

GameController_ErrorCode GamePadProxy::ButtonMenu_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonMenu, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::ButtonMenu_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonMenu);
}

GameController_ErrorCode GamePadProxy::ButtonHome_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonHome, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::ButtonHome_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonHome);
}

GameController_ErrorCode GamePadProxy::ButtonA_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonA, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::ButtonA_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonA);
}

GameController_ErrorCode GamePadProxy::ButtonB_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonB, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::ButtonB_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonB);
}

GameController_ErrorCode GamePadProxy::ButtonX_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonX, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::ButtonX_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonX);
}

GameController_ErrorCode GamePadProxy::ButtonY_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonY, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::ButtonY_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonY);
}

GameController_ErrorCode GamePadProxy::ButtonC_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonC, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::ButtonC_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::ButtonC);
}

GameController_ErrorCode GamePadProxy::Dpad_LeftButton_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::Dpad_LeftButton, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::Dpad_LeftButton_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::Dpad_LeftButton);
}

GameController_ErrorCode GamePadProxy::Dpad_RightButton_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::Dpad_RightButton, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::Dpad_RightButton_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::Dpad_RightButton);
}

GameController_ErrorCode GamePadProxy::Dpad_UpButton_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::Dpad_UpButton, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::Dpad_UpButton_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::Dpad_UpButton);
}

GameController_ErrorCode GamePadProxy::Dpad_DownButton_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::Dpad_DownButton, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::Dpad_DownButton_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::Dpad_DownButton);
}

GameController_ErrorCode GamePadProxy::Dpad_RegisterAxisInputMonitor(
    GamePad_AxisInputMonitorCallback inputMonitorCallback)
{
    return RegisterAxisInputMonitor(GamePadAxisSourceTypeEnum::Dpad, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::Dpad_UnRegisterAxisInputMonitor()
{
    return UnRegisterAxisInputMonitor(GamePadAxisSourceTypeEnum::Dpad);
}

GameController_ErrorCode GamePadProxy::LeftThumbstick_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::LeftThumbstickButton, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::LeftThumbstick_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::LeftThumbstickButton);
}

GameController_ErrorCode GamePadProxy::LeftThumbstick_RegisterAxisInputMonitor(
    GamePad_AxisInputMonitorCallback inputMonitorCallback)
{
    return RegisterAxisInputMonitor(GamePadAxisSourceTypeEnum::LeftThumbstick, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::LeftThumbstick_UnRegisterAxisInputMonitor()
{
    return UnRegisterAxisInputMonitor(GamePadAxisSourceTypeEnum::LeftThumbstick);
}

GameController_ErrorCode GamePadProxy::RightThumbstick_RegisterButtonInputMonitor(
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    return RegisterButtonInputMonitor(GamePadButtonTypeEnum::RightThumbstickButton, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::RightThumbstick_UnRegisterButtonInputMonitor()
{
    return UnRegisterButtonInputMonitor(GamePadButtonTypeEnum::RightThumbstickButton);
}

GameController_ErrorCode GamePadProxy::RightThumbstick_RegisterAxisInputMonitor(
    GamePad_AxisInputMonitorCallback inputMonitorCallback)
{
    return RegisterAxisInputMonitor(GamePadAxisSourceTypeEnum::RightThumbstick, inputMonitorCallback);
}

GameController_ErrorCode GamePadProxy::RightThumbstick_UnRegisterAxisInputMonitor()
{
    return UnRegisterAxisInputMonitor(GamePadAxisSourceTypeEnum::RightThumbstick);
}

GameController_ErrorCode GamePadProxy::RegisterButtonInputMonitor(
    GamePadButtonTypeEnum buttonTypeEnum,
    GamePad_ButtonInputMonitorCallback inputMonitorCallback)
{
    if (inputMonitorCallback == nullptr) {
        HILOGE("[CAPI][RegisterButtonInputMonitor]inputMonitorCallback is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }

    std::shared_ptr<GamePadButtonEventCallback> apiCallback = std::make_shared<GamePadButtonEventCallback>();
    apiCallback->SetCallback(inputMonitorCallback);
    InputEventClient::RegisterGamePadButtonEventCallback(ApiTypeEnum::CAPI,
                                                         buttonTypeEnum,
                                                         apiCallback);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadProxy::UnRegisterButtonInputMonitor(GamePadButtonTypeEnum buttonTypeEnum)
{
    InputEventClient::UnRegisterGamePadButtonEventCallback(ApiTypeEnum::CAPI,
                                                           buttonTypeEnum);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadProxy::RegisterAxisInputMonitor(GamePadAxisSourceTypeEnum axisTypeEnum,
                                                                GamePad_AxisInputMonitorCallback inputMonitorCallback)
{
    if (inputMonitorCallback == nullptr) {
        HILOGE("[CAPI][RegisterButtonInputMonitor]inputMonitorCallback is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }

    std::shared_ptr<GamePadAxisEventCallback> apiCallback = std::make_shared<GamePadAxisEventCallback>();
    apiCallback->SetCallback(inputMonitorCallback);
    InputEventClient::RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                       axisTypeEnum,
                                                       apiCallback);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GamePadProxy::UnRegisterAxisInputMonitor(GamePadAxisSourceTypeEnum axisTypeEnum)
{
    InputEventClient::UnRegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                         axisTypeEnum);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

int32_t GamePadButtonEventCallback::OnButtonEventCallback(const GamePadButtonEvent &event)
{
    if (callback_ == nullptr) {
        HILOGW("[capi]OnButtonEventCallback failed. callback is nullptr");
        return GAME_ERR_ARGUMENT_NULL;
    }
    GamePadButtonEvent* gamePadButtonEvent = new GamePadButtonEvent();
    gamePadButtonEvent->id = event.id;
    gamePadButtonEvent->uniq = event.uniq;
    gamePadButtonEvent->keyCode = event.keyCode;
    gamePadButtonEvent->keyCodeName = event.keyCodeName;
    gamePadButtonEvent->actionTime = event.actionTime;
    gamePadButtonEvent->keys = event.keys;
    gamePadButtonEvent->keyAction = event.keyAction;
    GamePad_ButtonEvent* temp = (GamePad_ButtonEvent*)gamePadButtonEvent;
    callback_(temp);
    if (gamePadButtonEvent != nullptr) {
        delete gamePadButtonEvent;
        gamePadButtonEvent = nullptr;
        temp = nullptr;
    }
    return GAME_CONTROLLER_SUCCESS;
}

void GamePadButtonEventCallback::SetCallback(GamePad_ButtonInputMonitorCallback callback)
{
    callback_ = callback;
}

int32_t GamePadAxisEventCallback::OnAxisEventCallback(const GamePadAxisEvent &event)
{
    if (callback_ == nullptr) {
        HILOGW("[capi]OnAxisEventCallback failed. callback is nullptr");
        return GAME_ERR_ARGUMENT_NULL;
    }
    GamePadAxisEvent* gamePadAxisEvent = new GamePadAxisEvent();
    gamePadAxisEvent->id = event.id;
    gamePadAxisEvent->uniq = event.uniq;
    gamePadAxisEvent->axisSourceType = event.axisSourceType;
    gamePadAxisEvent->xValue = event.xValue;
    gamePadAxisEvent->yValue = event.yValue;
    gamePadAxisEvent->zValue = event.zValue;
    gamePadAxisEvent->rzValue = event.rzValue;
    gamePadAxisEvent->brakeValue = event.brakeValue;
    gamePadAxisEvent->gasValue = event.gasValue;
    gamePadAxisEvent->hatxValue = event.hatxValue;
    gamePadAxisEvent->hatyValue = event.hatyValue;
    gamePadAxisEvent->actionTime = event.actionTime;
    GamePad_AxisEvent* temp = (GamePad_AxisEvent*)gamePadAxisEvent;
    callback_(temp);
    if (gamePadAxisEvent != nullptr) {
        delete gamePadAxisEvent;
        gamePadAxisEvent = nullptr;
        temp = nullptr;
    }
    return GAME_CONTROLLER_SUCCESS;
}

void GamePadAxisEventCallback::SetCallback(GamePad_AxisInputMonitorCallback callback)
{
    callback_ = callback;
}
}
}