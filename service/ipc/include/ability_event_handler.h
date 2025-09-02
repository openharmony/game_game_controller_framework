/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GAME_CONTROLLER_ABILITY_EVENT_HANDLER_H
#define GAME_CONTROLLER_ABILITY_EVENT_HANDLER_H

#include "event_handler.h"
#include "event_runner.h"
#include <unordered_map>
#include "ffrt.h"
#include "singleton.h"

namespace OHOS {
namespace GameController {
enum TaskId : uint32_t {
    UNLOAD_GAME_CONTROLLER = 1,
};
class AbilityEventHandler : public DelayedSingleton<AbilityEventHandler> {
    DECLARE_DELAYED_SINGLETON(AbilityEventHandler)

public:
    /**
     * Deferred Unloading
     */
    void DelayUnloadService();

private:
    void UnloadServiceTask();

private:
    ffrt::mutex mutex_;
    std::shared_ptr<AppExecFwk::EventHandler> unloadHandler_;
};
} // namespace GameController
} // namespace OHOS

#endif // GAME_CONTROLLER_ABILITY_EVENT_HANDLER_H