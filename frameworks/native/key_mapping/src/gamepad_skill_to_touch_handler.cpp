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

#include "gamepad_skill_to_touch_handler.h"
#include "key_to_touch_manager.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
void GamepadSkillToTouchHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                               const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                               const KeyToTouchMappingInfo &mappingInfo,
                                               const DeviceInfo &deviceInfo)
{
    if (context->isSkillOperating) {
        HILOGW("discard key event. It's skill-operating now");
        return;
    }
    HILOGI("gamepad key convert to down event of gamepad-skill-to-touch");
    DelayedSingleton<KeyToTouchManager>::GetInstance()->CancelStickTimers();
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context->SetCurrentSkillKeyInfo(mappingInfo, pointerId);
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(context->currentSkillKeyInfo, pointerId,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}

void GamepadSkillToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                             const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                             const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (!context->isSkillOperating) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. No skill-operating", keyCode);
        return;
    }
    if (context->currentSkillKeyInfo.keyCode != keyCode) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. keyCode is not current skill-operating keycode", keyCode);
        return;
    }
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_SKILL);
    if (!pair.first) {
        HILOGW("discard keyup event. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    if (context->pointerItems.find(pointerId) == context->pointerItems.end()) {
        HILOGW("discard button event, because cannot find the last point event");
        return;
    }
    PointerEvent::PointerItem lastMovePoint = context->pointerItems[pointerId];
    HILOGI("gamepad key convert to up event of gamepad-skill-to-touch");
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchUpEntity(lastMovePoint, pointerId,
                                                 PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    DelayedSingleton<KeyToTouchManager>::GetInstance()->ResetAxisHandlerStates();
    lastAxisZ_ = 0.0;
    lastAxisRZ_ = 0.0;
    context->ResetCurrentSkillKeyInfo();
}

void GamepadSkillToTouchHandler::ReadStickAxis(const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                               int32_t joystick)
{
    PointerEvent::AxisType axisZ;
    PointerEvent::AxisType axisRZ;
    if (joystick == 0) {
        axisZ = PointerEvent::AxisType::AXIS_TYPE_ABS_X;
        axisRZ = PointerEvent::AxisType::AXIS_TYPE_ABS_Y;
    } else {
        axisZ = PointerEvent::AxisType::AXIS_TYPE_ABS_Z;
        axisRZ = PointerEvent::AxisType::AXIS_TYPE_ABS_RZ;
    }
    if (pointerEvent->HasAxis(axisZ)) {
        lastAxisZ_ = pointerEvent->GetAxisValue(axisZ);
    }
    if (pointerEvent->HasAxis(axisRZ)) {
        lastAxisRZ_ = pointerEvent->GetAxisValue(axisRZ);
    }
}

void GamepadSkillToTouchHandler::SendSkillAimMove(
    std::shared_ptr<InputToTouchContext> &context,
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (lastAxisZ_ > -SKILL_DEAD_ZONE && lastAxisZ_ < SKILL_DEAD_ZONE
        && lastAxisRZ_ > -SKILL_DEAD_ZONE && lastAxisRZ_ < SKILL_DEAD_ZONE) {
        return;
    }
    int32_t aimX = context->currentSkillKeyInfo.xValue
        + static_cast<int32_t>(lastAxisZ_ * context->currentSkillKeyInfo.skillRange);
    int32_t aimY = context->currentSkillKeyInfo.yValue
        + static_cast<int32_t>(lastAxisRZ_ * context->currentSkillKeyInfo.skillRange);
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_SKILL);
    if (!pair.first) {
        HILOGW("discard axis event. because cannot find the pointerId for skill");
        return;
    }
    TouchEntity moveEntity;
    moveEntity.pointerId = pair.second;
    moveEntity.pointerAction = PointerEvent::POINTER_ACTION_MOVE;
    moveEntity.xValue = aimX;
    moveEntity.yValue = aimY;
    moveEntity.actionTime = pointerEvent->GetActionTime();
    BuildAndSendPointerEvent(context, moveEntity);
}

void GamepadSkillToTouchHandler::HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                                    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                    const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent == nullptr || context == nullptr) {
        HILOGW("pointerEvent or context is nullptr");
        return;
    }
    if (!context->isSkillOperating) {
        return;
    }
    int32_t action = pointerEvent->GetPointerAction();
    if (action != PointerEvent::POINTER_ACTION_AXIS_BEGIN
        && action != PointerEvent::POINTER_ACTION_AXIS_UPDATE
        && action != PointerEvent::POINTER_ACTION_AXIS_END) {
        return;
    }
    if (action == PointerEvent::POINTER_ACTION_AXIS_END) {
        lastAxisZ_ = 0.0;
        lastAxisRZ_ = 0.0;
        return;
    }
    ReadStickAxis(pointerEvent, context->currentSkillKeyInfo.joystick);
    SendSkillAimMove(context, pointerEvent);
}
}
}