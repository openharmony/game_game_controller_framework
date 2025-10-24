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
#include "mouse_right_key_walking_to_touch_handler.h"

namespace OHOS {
namespace GameController {

void MouseRightKeyWalkingToTouchHandler::HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                                            const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                            const KeyToTouchMappingInfo &mappingInfo)
{
    if (BaseKeyToTouchHandler::IsMouseRightButtonEvent(pointerEvent)) {
        if (HandleMouseRightBtnDown(context, pointerEvent, mappingInfo)) {
            return;
        }
        HandleMouseRightBtnUp(context, pointerEvent);
        return;
    }
    if (BaseKeyToTouchHandler::IsMouseMoveEvent(pointerEvent)) {
        HandleMouseMove(context, pointerEvent);
    }
}

bool MouseRightKeyWalkingToTouchHandler::HandleMouseRightBtnDown(std::shared_ptr<InputToTouchContext> &context,
                                                                 const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                                 const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent->GetPointerAction() != PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        return false;
    }

    if (context->isWalking) {
        return true;
    }
    HILOGI("Enter walking by mouse-right-key-walking");
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context->SetCurrentWalking(mappingInfo, pointerId);
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(mappingInfo, pointerId,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    HandleMouseMove(context, pointerEvent);
    return true;
}

void MouseRightKeyWalkingToTouchHandler::HandleMouseRightBtnUp(std::shared_ptr<InputToTouchContext> &context,
                                                               const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (pointerEvent->GetPointerAction() != PointerEvent::POINTER_ACTION_BUTTON_UP) {
        return;
    }
    if (!context->IsMouseRightWalking()) {
        return;
    }
    HILOGI("Exit walking by mouse-right-key-walking");
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_WALK);
    if (!pair.first) {
        HILOGW("discard mouse-right up event. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(context->currentWalking, pointerId,
                                               PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->ResetCurrentWalking();
}

void MouseRightKeyWalkingToTouchHandler::HandleMouseMove(std::shared_ptr<InputToTouchContext> &context,
                                                         const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (!context->IsMouseRightWalking()) {
        return;
    }

    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_WALK);
    if (!pair.first) {
        HILOGW("discard mouse move event. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;

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

    // compute the angle with window's center point and mouse's point
    double angle = CalculateAngle(centerPoint, mousePoint);

    // compute the target point
    Point dpadPoint;
    dpadPoint.x = context->currentWalking.xValue;
    dpadPoint.y = context->currentWalking.yValue;
    Point targetPoint = ComputeTargetPoint(dpadPoint, context->currentWalking.radius, angle);

    // move to the target
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildMoveTouchEntity(pointerId, targetPoint, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}
}
}