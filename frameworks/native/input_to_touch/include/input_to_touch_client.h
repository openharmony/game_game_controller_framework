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

#ifndef GAME_CONTROLLER_FRAMEWORK_INPUT_TO_TOUCH_CLIENT_H
#define GAME_CONTROLLER_FRAMEWORK_INPUT_TO_TOUCH_CLIENT_H

#include <common_event_subscriber.h>
#include <common_event_data.h>
#include "gamecontroller_keymapping_model.h"

namespace OHOS {
namespace GameController {
/**
 * 游戏公共事件的监听回调
 */
class GameCommonEventListener : public EventFwk::CommonEventSubscriber,
                                public std::enable_shared_from_this<GameCommonEventListener> {
public:
    explicit GameCommonEventListener(const EventFwk::CommonEventSubscribeInfo &subscriberInfo)
        : EventFwk::CommonEventSubscriber(subscriberInfo)
    {
    }

    ~GameCommonEventListener() = default;

    void OnReceiveEvent(const EventFwk::CommonEventData &data) override;
};

class InputToTouchClient {
public:
    static void StartInputToTouch();

private:
    static void DoAsyncTask();

    static bool IsCommonApp();

    static bool IsSupportKeyMapping();

    static void StartInputMonitor();

    static void StartPublicEventMonitor();

private:
    static std::shared_ptr<GameCommonEventListener> subscriber_;
};
}
}
#endif //GAME_CONTROLLER_FRAMEWORK_INPUT_TO_TOUCH_CLIENT_H
