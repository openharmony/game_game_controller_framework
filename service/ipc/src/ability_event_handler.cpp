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

#include "ability_event_handler.h"

#include "datetime_ex.h"
#include "system_ability.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "gamecontroller_log.h"
#include "gamecontroller_constants.h"

using namespace OHOS::AppExecFwk;
namespace OHOS {
namespace GameController {
namespace {
static const int32_t UNLOAD_DELAY_TIME = 30000; // Delayed offloading duration, in ms.
}
AbilityEventHandler::AbilityEventHandler()
{
    HILOGI("AbilityEventHandler instance is created");
    if (unloadHandler_ == nullptr) {
        auto runner = EventRunner::Create(true, ThreadMode::FFRT);
        unloadHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }
}

AbilityEventHandler::~AbilityEventHandler()
{
    HILOGI("AbilityEventHandler instance is destroyed");
}

void AbilityEventHandler::DelayUnloadService()
{
    if (unloadHandler_ == nullptr) {
        HILOGI("DelayUnload failed, unloadHandler_ is null");
        return;
    }
    HILOGI("Will unload after 30 s");
    unloadHandler_->RemoveTask(std::to_string(TaskId::UNLOAD_GAME_CONTROLLER));
    auto task = [=]() { UnloadServiceTask(); };
    unloadHandler_->PostTask(task, std::to_string(TaskId::UNLOAD_GAME_CONTROLLER), UNLOAD_DELAY_TIME);
}

void AbilityEventHandler::UnloadServiceTask()
{
    HILOGI("UnloadServiceTask begin");
    std::lock_guard <ffrt::mutex> lock(mutex_);

    auto saMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saMgr == nullptr) {
        HILOGE("get saMgr failed");
        return;
    }
    int32_t ret = saMgr->UnloadSystemAbility(GAME_CONTROLLER_SA_ID);
    if (ret != ERR_OK) {
        HILOGE("UnloadSystemAbility failed");
        return;
    }
    HILOGI("UnloadServiceTask End");
}

}  // namespace GameController
}  // namespace OHOS
