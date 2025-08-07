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

#include "input_event_callback.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
InputEventCallback::~InputEventCallback()
{
}

InputEventCallback::InputEventCallback()
{
}

void InputEventCallback::RegisterGamePadButtonEventCallback(const ApiTypeEnum apiTypeEnum,
                                                            const GamePadButtonTypeEnum gamePadButtonTypeEnum,
                                                            const std::shared_ptr<GamePadButtonCallbackBase> &callback)
{
    if (callback == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(registerButtonMutex_);
    HILOGI("[InputEventCallback]RegisterGamePadButtonEventCallback ApiTypeEnum is %{public}d, "
           "ButtonTypeEnum is %{public}d", apiTypeEnum, gamePadButtonTypeEnum);
    gamePadButtonCallback_[gamePadButtonTypeEnum] = callback;
}

void InputEventCallback::UnRegisterGamePadButtonEventCallback(const ApiTypeEnum apiTypeEnum,
                                                              const GamePadButtonTypeEnum gamePadButtonTypeEnum)
{
    std::lock_guard<std::mutex> lock(registerButtonMutex_);
    if (gamePadButtonCallback_.find(gamePadButtonTypeEnum) == gamePadButtonCallback_.end()) {
        return;
    }
    HILOGI("[InputEventCallback]UnRegisterGamePadButtonEventCallback ApiTypeEnum is %{public}d, "
           "ButtonTypeEnum is %{public}d", apiTypeEnum, gamePadButtonTypeEnum);
    gamePadButtonCallback_.erase(gamePadButtonTypeEnum);
}

void InputEventCallback::RegisterGamePadAxisEventCallback(const ApiTypeEnum apiTypeEnum,
                                                          const GamePadAxisSourceTypeEnum gamePadAxisTypeEnum,
                                                          const std::shared_ptr<GamePadAxisCallbackBase> &callback)
{
    if (callback == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(registerAxisMutex_);
    HILOGI("[InputEventCallback]RegisterGamePadAxisEventCallback ApiTypeEnum is %{public}d, "
           "AxisTypeEnum is %{public}d", apiTypeEnum, gamePadAxisTypeEnum);
    gamePadAxisCallback_[gamePadAxisTypeEnum] = callback;
}

void InputEventCallback::UnRegisterGamePadAxisEventCallback(const ApiTypeEnum apiTypeEnum,
                                                            const GamePadAxisSourceTypeEnum gamePadAxisTypeEnum)
{
    std::lock_guard<std::mutex> lock(registerAxisMutex_);
    if (gamePadAxisCallback_.find(gamePadAxisTypeEnum) == gamePadAxisCallback_.end()) {
        return;
    }
    HILOGI("[InputEventCallback]UnRegisterGamePadAxisEventCallback ApiTypeEnum is %{public}d, "
           "AxisTypeEnum is %{public}d", apiTypeEnum, gamePadAxisTypeEnum);
    gamePadAxisCallback_.erase(gamePadAxisTypeEnum);
}

void InputEventCallback::OnGamePadAxisEventCallback(const GamePadAxisEvent &event)
{
    std::shared_ptr<GamePadAxisCallbackBase> axisCallback = nullptr;
    {
        std::lock_guard<std::mutex> lock(registerAxisMutex_);
        if (gamePadAxisCallback_.empty()) {
            return;
        }
        if (gamePadAxisCallback_.find(event.axisSourceType) == gamePadAxisCallback_.end()) {
            HILOGD("[InputEventCallback]OnGamePadAxisEventCallback. No callback on axisType [%{public}d]",
                   event.axisSourceType);
            return;
        }
        axisCallback = gamePadAxisCallback_[event.axisSourceType];
    }
    if (axisCallback == nullptr) {
        return;
    }
    axisCallback->OnAxisEventCallback(event);
}

void InputEventCallback::OnGamePadButtonEventCallback(const GamePadButtonEvent &event)
{
    std::shared_ptr<GamePadButtonCallbackBase> buttonCallback = nullptr;
    {
        std::lock_guard<std::mutex> lock(registerButtonMutex_);
        if (gamePadButtonCallback_.empty()) {
            return;
        }
        GamePadButtonTypeEnum gamePadButtonTypeEnum = static_cast<GamePadButtonTypeEnum>(event.keyCode);
        if (gamePadButtonCallback_.find(gamePadButtonTypeEnum) == gamePadButtonCallback_.end()) {
            HILOGD("[InputEventCallback]OnGamePadButtonEventCallback. No callback on keyCode [%{private}d]",
                   event.keyCode);
            return;
        }
        buttonCallback = gamePadButtonCallback_[gamePadButtonTypeEnum];
    }
    if (buttonCallback == nullptr) {
        return;
    }
    buttonCallback->OnButtonEventCallback(event);
}
}
}