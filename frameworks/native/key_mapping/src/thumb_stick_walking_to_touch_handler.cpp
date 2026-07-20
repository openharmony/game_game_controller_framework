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

#include "thumb_stick_walking_to_touch_handler.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
void ThumbStickWalkingToTouchHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                                    const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                    const KeyToTouchMappingInfo &mappingInfo,
                                                    const DeviceInfo &deviceInfo)
{
}

void ThumbStickWalkingToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                                  const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                  const DeviceInfo &deviceInfo)
{
}

void ThumbStickWalkingToTouchHandler::HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                                         const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                         const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent == nullptr || context == nullptr) {
        HILOGW("pointerEvent or context is nullptr");
        return;
    }
    int32_t action = pointerEvent->GetPointerAction();
    if (action != PointerEvent::POINTER_ACTION_AXIS_BEGIN
        && action != PointerEvent::POINTER_ACTION_AXIS_UPDATE
        && action != PointerEvent::POINTER_ACTION_AXIS_END) {
        return;
    }
    HandleAxisEvent(context, pointerEvent, mappingInfo);
}

void ThumbStickWalkingToTouchHandler::GetStickAxisTypes(
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    int32_t joystick,
    PointerEvent::AxisType &axisX,
    PointerEvent::AxisType &axisY) const
{
    if (joystick == 0) {
        axisX = PointerEvent::AxisType::AXIS_TYPE_ABS_X;
        axisY = PointerEvent::AxisType::AXIS_TYPE_ABS_Y;
    } else {
        axisX = PointerEvent::AxisType::AXIS_TYPE_ABS_Z;
        axisY = PointerEvent::AxisType::AXIS_TYPE_ABS_RZ;
    }
}

void ThumbStickWalkingToTouchHandler::DeactivatePointer(
    std::shared_ptr<InputToTouchContext> &context, int64_t actionTime)
{
    if (context->pointerItems.find(pointerId_) != context->pointerItems.end()) {
        PointerEvent::PointerItem lastItem = context->pointerItems[pointerId_];
        TouchEntity upEntity = BuildTouchUpEntity(lastItem, pointerId_,
                                                  PointerEvent::POINTER_ACTION_UP, actionTime);
        BuildAndSendPointerEvent(context, upEntity);
    }
    DelayedSingleton<PointerManager>::GetInstance()->ReleasePointerId(pointerId_);
    isActive_ = false;
}

void ThumbStickWalkingToTouchHandler::UpdateStickAxes(
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    const KeyToTouchMappingInfo &mappingInfo)
{
    PointerEvent::AxisType axisX;
    PointerEvent::AxisType axisY;
    GetStickAxisTypes(pointerEvent, mappingInfo.joystick, axisX, axisY);
    if (pointerEvent->HasAxis(axisX)) {
        lastAxisX_ = pointerEvent->GetAxisValue(axisX);
    }
    if (pointerEvent->HasAxis(axisY)) {
        lastAxisY_ = pointerEvent->GetAxisValue(axisY);
    }
}

void ThumbStickWalkingToTouchHandler::HandleAxisEvent(std::shared_ptr<InputToTouchContext> &context,
                                                      const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                      const KeyToTouchMappingInfo &mappingInfo)
{
    int32_t action = pointerEvent->GetPointerAction();
    int64_t actionTime = pointerEvent->GetActionTime();

    if (action == PointerEvent::POINTER_ACTION_AXIS_END) {
        if (isActive_) {
            DeactivatePointer(context, actionTime);
            lastAxisX_ = 0.0;
            lastAxisY_ = 0.0;
        }
        return;
    }

    UpdateStickAxes(pointerEvent, mappingInfo);

    bool inDeadZone = (lastAxisX_ > -DEAD_ZONE && lastAxisX_ < DEAD_ZONE
                       && lastAxisY_ > -DEAD_ZONE && lastAxisY_ < DEAD_ZONE);

    if (isActive_ && inDeadZone) {
        DeactivatePointer(context, actionTime);
        return;
    }

    if (!inDeadZone) {
        int32_t targetX = mappingInfo.xValue + static_cast<int32_t>(lastAxisX_ * mappingInfo.radius);
        int32_t targetY = mappingInfo.yValue + static_cast<int32_t>(lastAxisY_ * mappingInfo.radius);
        if (!isActive_) {
            pointerId_ = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
            isActive_ = true;
            TouchEntity touchEntity = BuildTouchEntity(mappingInfo, pointerId_,
                                                       PointerEvent::POINTER_ACTION_DOWN, actionTime);
            touchEntity.xValue = targetX;
            touchEntity.yValue = targetY;
            BuildAndSendPointerEvent(context, touchEntity);
        } else {
            TouchEntity touchEntity;
            touchEntity.pointerId = pointerId_;
            touchEntity.pointerAction = PointerEvent::POINTER_ACTION_MOVE;
            touchEntity.xValue = targetX;
            touchEntity.yValue = targetY;
            touchEntity.actionTime = actionTime;
            BuildAndSendPointerEvent(context, touchEntity);
        }
    }
}
}
}
