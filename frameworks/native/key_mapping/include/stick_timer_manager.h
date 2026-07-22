/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef GAME_CONTROLLER_FRAMEWORK_STICK_TIMER_MANAGER_H
#define GAME_CONTROLLER_FRAMEWORK_STICK_TIMER_MANAGER_H

#include <functional>
#include <memory>
#include <unordered_map>
#include "ffrt.h"
#include "singleton.h"

namespace OHOS {
namespace GameController {

enum StickTimerId : int32_t {
    STICK_TIMER_OBSERVATION = 0,
    STICK_TIMER_FPS = 1,
};

class StickTimerManager : public DelayedSingleton<StickTimerManager> {
    DECLARE_DELAYED_SINGLETON(StickTimerManager)

public:
    void Start(StickTimerId timerId, std::function<void()> tick, int32_t intervalMs);
    void Cancel(StickTimerId timerId);
    bool IsRunning(StickTimerId timerId) const;

private:
    struct TimerEntry {
        std::atomic<bool> running{false};
        std::atomic<bool> stopped{false};
        std::unique_ptr<std::function<void()>> tick;
        int32_t intervalMs = 0;
        ffrt_timer_t handle;
    };

    static void TimerCallback(void* arg);

    std::unordered_map<int32_t, std::shared_ptr<TimerEntry>> timers_;
    mutable ffrt::mutex mutex_;
};

} // namespace GameController
} // namespace OHOS

#endif // GAME_CONTROLLER_FRAMEWORK_STICK_TIMER_MANAGER_H