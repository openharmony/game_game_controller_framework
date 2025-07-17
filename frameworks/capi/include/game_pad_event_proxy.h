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

#ifndef GAME_CONTROLLER_FRAMEWORK_GAME_PAD_EVENT_PROXY_H
#define GAME_CONTROLLER_FRAMEWORK_GAME_PAD_EVENT_PROXY_H

#include <singleton.h>
#include "game_pad_event.h"

namespace OHOS {
namespace GameController {
class GamePadEventProxy : public Singleton<GamePadEventProxy> {
DECLARE_SINGLETON(GamePadEventProxy);
public:
    GameController_ErrorCode GetDeviceIdFromButtonEvent(const struct GamePad_ButtonEvent* buttonEvent,
                                                        char** deviceId);

    GameController_ErrorCode GetButtonActionFromButtonEvent(const struct GamePad_ButtonEvent* buttonEvent,
                                                            GamePad_Button_ActionType* actionType);

    GameController_ErrorCode GetButtonCodeFromButtonEvent(const struct GamePad_ButtonEvent* buttonEvent,
                                                          int32_t* code);

    GameController_ErrorCode GetButtonCodeNameFromButtonEvent(const struct GamePad_ButtonEvent* buttonEvent,
                                                              char** codeName);

    GameController_ErrorCode GetCountFromPressedButtons(const struct GamePad_ButtonEvent* buttonEvent,
                                                        int32_t* count);

    GameController_ErrorCode GetButtonInfoFromPressedButtons(const struct GamePad_ButtonEvent* buttonEvent,
                                                             const int32_t index,
                                                             GamePad_PressedButton** pressedButton);

    GameController_ErrorCode DestroyPressedButton(GamePad_PressedButton** pressedButton);

    GameController_ErrorCode GetButtonCodeFromPressedButton(const struct GamePad_PressedButton* pressedButton,
                                                            int32_t* code);

    GameController_ErrorCode GetButtonCodeNameFromPressedButton(const struct GamePad_PressedButton* pressedButton,
                                                                char** codeName);

    GameController_ErrorCode GetActionTimeFromButtonEvent(const struct GamePad_ButtonEvent* buttonEvent,
                                                          int64_t* actionTime);

    GameController_ErrorCode GetDeviceIdFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                      char** deviceId);

    GameController_ErrorCode GetAxisSourceTypeFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                            GamePad_AxisSourceType* axisSourceType);

    GameController_ErrorCode GetXAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                        double* axisValue);

    GameController_ErrorCode GetYAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                        double* axisValue);

    GameController_ErrorCode GetZAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                        double* axisValue);

    GameController_ErrorCode GetRZAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                         double* axisValue);

    GameController_ErrorCode GetHatXAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                           double* axisValue);

    GameController_ErrorCode GetHatYAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                           double* axisValue);

    GameController_ErrorCode GetGasAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                          double* axisValue);

    GameController_ErrorCode GetBrakeAxisValueFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                            double* axisValue);

    GameController_ErrorCode GetActionTimeFromAxisEvent(const struct GamePad_AxisEvent* axisEvent,
                                                        int64_t* actionTime);
};
}
}
#endif //GAME_CONTROLLER_FRAMEWORK_GAME_PAD_EVENT_PROXY_H
