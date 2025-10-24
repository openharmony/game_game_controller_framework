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

#include <window_input_intercept_client.h>
#include "skill_key_to_touch_handler.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
void SkillKeyToTouchHandler::HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                                const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent == nullptr) {
        HILOGW("pointerEvent is nullptr");
        return;
    }
    if (IsNeedHandlePointEvent(context, pointerEvent)) {
        HandleMouseMove(context, pointerEvent);
    }
    Rosen::WindowInputInterceptClient::SendInputEvent(pointerEvent);
}

void SkillKeyToTouchHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                           const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                           const KeyToTouchMappingInfo &mappingInfo,
                                           const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (context->isSkillOperating) {
        if (context->currentSkillKeyInfo.keyCode != keyCode) {
            HILOGW("discard keyCode [%{private}d]. It's skill-operating now", keyCode);
        }
        return;
    }

    HILOGI("keyCode [%{private}d] convert to down event of skill-to-touch", keyCode);
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context->SetCurrentSkillKeyInfo(mappingInfo, pointerId);
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(context->currentSkillKeyInfo, pointerId,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}

void SkillKeyToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                         const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                         const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (!context->isSkillOperating) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. No skill-operating",
               keyCode);
        return;
    }
    if (context->currentSkillKeyInfo.keyCode != keyCode) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. keyCode is not current skill-operating keycode",
               keyCode);
        return;
    }
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_SKILL);
    if (!pair.first) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. because cannot find the pointerId", keyCode);
        return;
    }
    int32_t pointerId = pair.second;
    if (context->pointerItems.find(pointerId) == context->pointerItems.end()) {
        HILOGW("discard button event, because cannot find the last point event");
        return;
    }
    PointerEvent::PointerItem lastMovePoint = context->pointerItems[pointerId];

    HILOGI("keyCode [%{private}d] convert to up event of skill-to-touch", keyCode);
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchUpEntity(lastMovePoint, pointerId,
                                                 PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->ResetCurrentSkillKeyInfo();
}

void SkillKeyToTouchHandler::HandleMouseMove(std::shared_ptr<InputToTouchContext> &context,
                                             const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    // get the window's center point
    Point centerPoint;
    centerPoint.x = static_cast<double>(context->windowInfoEntity.xCenter);
    centerPoint.y = static_cast<double>(context->windowInfoEntity.yCenter);

    // get the mouse's point in window
    Point mousePoint;
    PointerEvent::PointerItem pointerItem;
    pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem);
    mousePoint.x = pointerItem.GetWindowX();
    mousePoint.y = pointerItem.GetWindowY();

    // compute the distance and angle with window's center point and mouse's point
    double distance = CalculateDistance(centerPoint, mousePoint);
    double angle = CalculateAngle(centerPoint, mousePoint);

    // compute the move point in skill key range
    double radius;
    if (distance >= context->currentSkillKeyInfo.skillRange) {
        radius = context->currentSkillKeyInfo.radius;
    } else {
        radius = (distance * context->currentSkillKeyInfo.radius) / context->currentSkillKeyInfo.skillRange;
    }
    Point skillCenterPoint;
    skillCenterPoint.x = context->currentSkillKeyInfo.xValue;
    skillCenterPoint.y = context->currentSkillKeyInfo.yValue;
    Point targetPoint = ComputeTargetPoint(skillCenterPoint, radius, angle);

    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_SKILL);
    if (!pair.first) {
        HILOGW("discard mouse move event. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildMoveTouchEntity(pointerId, targetPoint, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}

bool SkillKeyToTouchHandler::IsNeedHandlePointEvent(std::shared_ptr<InputToTouchContext> &context,
                                                    const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (!BaseKeyToTouchHandler::IsMouseMoveEvent(pointerEvent)) {
        return false;
    }
    if (!context->isSkillOperating) {
        return false;
    }
    return true;
}
}
}