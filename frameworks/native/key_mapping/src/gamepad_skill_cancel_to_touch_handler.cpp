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

#include "gamepad_skill_cancel_to_touch_handler.h"
#include "key_to_touch_manager.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
namespace {
const int64_t DELAY_MS_TO_US = 1000; // 1ms = 1000us
}

void GamepadSkillCancelToTouchHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                                     const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                     const KeyToTouchMappingInfo &mappingInfo,
                                                     const DeviceInfo &deviceInfo)
{
    if (!context->isSkillOperating) {
        HILOGW("discard cancel key. No skill-operating");
        return;
    }
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_SKILL);
    if (!pair.first) {
        HILOGW("discard cancel key. cannot find skill pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    int32_t toX = mappingInfo.xValue;
    int32_t toY = mappingInfo.yValue;

    TouchEntity moveEntity;
    moveEntity.pointerId = pointerId;
    moveEntity.pointerAction = PointerEvent::POINTER_ACTION_MOVE;
    moveEntity.xValue = toX;
    moveEntity.yValue = toY;
    moveEntity.actionTime = keyEvent->GetActionTime();
    BuildAndSendPointerEvent(context, moveEntity);

    int64_t delayUs = static_cast<int64_t>(mappingInfo.delayTime) * DELAY_MS_TO_US;
    if (delayUs > 0) {
        HILOGI("Gamepad skill cancel: MOVE to(%d,%d), UP in %lldus",
               toX, toY, static_cast<long long>(delayUs));
        ffrt::this_task::sleep_for(std::chrono::microseconds(delayUs));
    } else {
        HILOGI("Gamepad skill cancel: MOVE to(%d,%d), UP immediately", toX, toY);
    }

    SendUpAndCleanup(context, keyEvent, pointerId);
}

void GamepadSkillCancelToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                                   const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                   const DeviceInfo &deviceInfo)
{
}

void GamepadSkillCancelToTouchHandler::SendUpAndCleanup(
    std::shared_ptr<InputToTouchContext> &context,
    const std::shared_ptr<MMI::KeyEvent> &keyEvent,
    int32_t pointerId)
{
    if (context->pointerItems.find(pointerId) == context->pointerItems.end()) {
        HILOGW("discard cancel key, because cannot find the last point event");
        return;
    }
    PointerEvent::PointerItem lastMovePoint = context->pointerItems[pointerId];
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchUpEntity(lastMovePoint, pointerId,
                                                 PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);

    DelayedSingleton<KeyToTouchManager>::GetInstance()->ResetAxisHandlerStates();
    context->ResetCurrentSkillKeyInfo();
}
}
}
