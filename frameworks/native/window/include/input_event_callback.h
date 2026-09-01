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

#ifndef GAME_CONTROLLER_FRAMEWORK_INPUT_EVENT_CALLBACK_H
#define GAME_CONTROLLER_FRAMEWORK_INPUT_EVENT_CALLBACK_H

#include <cpp/mutex.h>
#include <singleton.h>
#include <unordered_map>
#include "gamecontroller_client_model.h"

namespace OHOS {
namespace GameController {
class InputEventCallback : public DelayedSingleton<InputEventCallback> {
DECLARE_DELAYED_SINGLETON(InputEventCallback);

public:
    /**
     * Registering the Gamepad button Callback Event
     * @param apiTypeEnum API Source
     * @param gamePadButtonTypeEnum Button Type
     * @param callback Callback Method
     */
    void RegisterGamePadButtonEventCallback(const ApiTypeEnum apiTypeEnum,
                                            const GamePadButtonTypeEnum gamePadButtonTypeEnum,
                                            const std::shared_ptr<GamePadButtonCallbackBase> &callback);

    /**
     * Cancel the callback event of Gamepad button.
     * @param gamePadButtonTypeEnum Button Type
     * @param apiTypeEnum API Source
     */
    void UnRegisterGamePadButtonEventCallback(const ApiTypeEnum apiTypeEnum,
                                              const GamePadButtonTypeEnum gamePadButtonTypeEnum);

    /**
     * Registering a Gamepad Axis Callback Event
     * @param apiTypeEnum API Source
     * @param gamePadAxisTypeEnum Axis Type
     * @param callback Callback Method
     */
    void RegisterGamePadAxisEventCallback(const ApiTypeEnum apiTypeEnum,
                                          const GamePadAxisSourceTypeEnum gamePadAxisTypeEnum,
                                          const std::shared_ptr<GamePadAxisCallbackBase> &callback);

    /**
     * Cancel the Gamepad axis callback event.
     * @param apiTypeEnum API Source
     * @param gamePadAxisTypeEnum Axis Type
     */
    void UnRegisterGamePadAxisEventCallback(const ApiTypeEnum apiTypeEnum,
                                            const GamePadAxisSourceTypeEnum gamePadAxisTypeEnum);

    /**
     * Perform Axis Event Callback
     * @param event Axis Event
     */
    void OnGamePadAxisEventCallback(const GamePadAxisEvent &event);

    /**
     * Perform Button Event Callback
     * @param event Button Event
     */
    void OnGamePadButtonEventCallback(const GamePadButtonEvent &event);

    /**
     * Registering the unknown button Callback Event
     * @param apiTypeEnum API Source
     * @param callback Callback Method
     */
    void RegisterUnknownButtonEventCallback(const ApiTypeEnum apiTypeEnum,
                                            const std::shared_ptr<GamePadButtonCallbackBase> &callback);

    /**
     * Cancel the callback event of unknown button.
     * @param apiTypeEnum API Source
     */
    void UnRegisterUnknownButtonEventCallback(const ApiTypeEnum apiTypeEnum);

    /**
     * Perform Unknown Button Event Callback
     * @param event Button Event
     */
    void OnGamePadUnknownButtonEventCallback(const GamePadButtonEvent &event);

private:
    ffrt::mutex registerAxisMutex_;

    ffrt::mutex registerButtonMutex_;

    ffrt::mutex registerUnknownButtonMutex_;

    /**
     * Callback method cache for axis
     */
    std::unordered_map<GamePadAxisSourceTypeEnum, std::shared_ptr<GamePadAxisCallbackBase>> gamePadAxisCallback_;

    /**
     * Callback method cache for button
     */
    std::unordered_map<GamePadButtonTypeEnum, std::shared_ptr<GamePadButtonCallbackBase>> gamePadButtonCallback_;

    /**
     * Callback method cache for unknown button
     */
    std::shared_ptr<GamePadButtonCallbackBase> gamePadUnknownButtonCallback_;
};

}// namespace GameController
}// namespace OHOS

#endif//GAME_CONTROLLER_FRAMEWORK_INPUT_EVENT_CALLBACK_H
