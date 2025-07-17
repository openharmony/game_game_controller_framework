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
#ifndef GAME_CONTROLLER_FRAMEWORK_EVENT_CALLBACK_H
#define GAME_CONTROLLER_FRAMEWORK_EVENT_CALLBACK_H

#include <cstdint>
#include "gamecontroller_client_model.h"

namespace OHOS {
namespace GameController {
class GamePadButtonEventCallback : public GamePadButtonCallbackBase {
public:
    GamePadButtonEventCallback() = default;

    ~GamePadButtonEventCallback() = default;

    int32_t OnButtonEventCallback(const GamePadButtonEvent &event) override
    {
        result_ = event;
        return 0;
    }

public:
    GamePadButtonEvent result_;
};

class GamePadAxisEventCallback : public GamePadAxisCallbackBase {
public:
    GamePadAxisEventCallback() = default;

    ~GamePadAxisEventCallback() = default;

    int32_t OnAxisEventCallback(const GamePadAxisEvent &event) override
    {
        result_ = event;
        return 0;
    }

public:
    GamePadAxisEvent result_;
};
}
}
#endif //GAME_CONTROLLER_FRAMEWORK_EVENT_CALLBACK_H
