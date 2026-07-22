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

#include "stick_timer_manager.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
namespace {
constexpr int32_t CANCEL_TIMEOUT_MS = 500;
constexpr int32_t CANCEL_POLL_MS = 5;
}

StickTimerManager::StickTimerManager() = default;
StickTimerManager::~StickTimerManager() = default;

void StickTimerManager::TimerCallback(void* arg)
{
    auto* entry = static_cast<TimerEntry*>(arg);
    if (!entry->running.load()) {
        entry->stopped.store(true);
        return;
    }
    (*entry->tick)();
    entry->handle = ffrt_timer_start(ffrt_qos_default,
        static_cast<uint64_t>(entry->intervalMs), entry, TimerCallback, false);
}

void StickTimerManager::Start(StickTimerId timerId, std::function<void()> tick, int32_t intervalMs)
{
    int32_t id = static_cast<int32_t>(timerId);
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto &entry = timers_[id];
    if (entry == nullptr) {
        entry = std::make_shared<TimerEntry>();
    }
    if (entry->running.load()) {
        ffrt_timer_stop(ffrt_qos_default, entry->handle);
    }
    entry->tick = std::make_unique<std::function<void()>>(std::move(tick));
    entry->intervalMs = intervalMs;
    entry->stopped.store(false);
    entry->running.store(true);
    entry->handle = ffrt_timer_start(ffrt_qos_default,
        static_cast<uint64_t>(intervalMs), entry.get(), TimerCallback, false);
}

void StickTimerManager::Cancel(StickTimerId timerId)
{
    int32_t id = static_cast<int32_t>(timerId);
    std::shared_ptr<TimerEntry> entry;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        auto it = timers_.find(id);
        if (it == timers_.end() || it->second == nullptr) {
            return;
        }
        entry = it->second;
    }
    entry->running.store(false);
    ffrt_timer_stop(ffrt_qos_default, entry->handle);

    // Yield-based wait: ffrt::this_task::sleep_for yields the ffrt worker
    // to allow any in-flight TimerCallback to run and set stopped=true,
    // preventing the deadlock that std::this_thread::sleep_for would cause.
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(CANCEL_TIMEOUT_MS);
    while (!entry->stopped.load()) {
        if (std::chrono::steady_clock::now() > deadline) {
            HILOGW("StickTimerManager: timer %d callback did not complete within %dms",
                   id, CANCEL_TIMEOUT_MS);
            break;
        }
        ffrt::this_task::sleep_for(std::chrono::milliseconds(CANCEL_POLL_MS));
    }
    entry->stopped.store(false);
    entry->tick.reset();
}

bool StickTimerManager::IsRunning(StickTimerId timerId) const
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto it = timers_.find(static_cast<int32_t>(timerId));
    if (it == timers_.end() || it->second == nullptr) {
        return false;
    }
    return it->second->running.load();
}

} // namespace GameController
} // namespace OHOS