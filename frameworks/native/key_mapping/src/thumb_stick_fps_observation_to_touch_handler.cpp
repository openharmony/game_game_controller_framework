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

#include "thumb_stick_fps_observation_to_touch_handler.h"
#include "gamecontroller_log.h"
#include <algorithm>
#include <cmath>

namespace OHOS {
namespace GameController {

ThumbStickFpsObservationToTouchHandler::~ThumbStickFpsObservationToTouchHandler()
{
    CancelTimer();
}

void ThumbStickFpsObservationToTouchHandler::ResetState()
{
    CancelTimer();
    lastAxisZ_ = 0.0;
    lastAxisRZ_ = 0.0;
}

void ThumbStickFpsObservationToTouchHandler::CancelTimer()
{
    task_.StopTimer();
}

void ThumbStickFpsObservationToTouchHandler::ReleaseIfActive(
    std::shared_ptr<InputToTouchContext> &context)
{
    if (task_.IsActive()) {
        DeactivateFpsObservation(context, 0);
    } else {
        ResetState();
    }
}

void ThumbStickFpsObservationToTouchHandler::HandlePointerEvent(
    std::shared_ptr<InputToTouchContext> &context,
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent == nullptr || context == nullptr) { return; }
    int32_t action = pointerEvent->GetPointerAction();
    if (action != PointerEvent::POINTER_ACTION_AXIS_BEGIN
        && action != PointerEvent::POINTER_ACTION_AXIS_UPDATE
        && action != PointerEvent::POINTER_ACTION_AXIS_END) { return; }
    HandleAxisEvent(context, pointerEvent, mappingInfo);
}

void ThumbStickFpsObservationToTouchHandler::ActivateFpsObservation(
    std::shared_ptr<InputToTouchContext> &context,
    const KeyToTouchMappingInfo &mappingInfo,
    int64_t actionTime,
    double rawZ, double rawRZ, double rawMag)
{
    int32_t baseSpeedX = (mappingInfo.xStep > 0) ? mappingInfo.xStep : DEFAULT_BASE_SPEED;
    int32_t baseSpeedY = (mappingInfo.yStep > 0) ? mappingInfo.yStep
        : static_cast<int32_t>(baseSpeedX * Y_AXIS_RATIO);

    pointerId_ = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    TouchEntity downEntity = BuildTouchEntity(mappingInfo, pointerId_,
                                              PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, downEntity);

    // Compute initial position
    int32_t curX = mappingInfo.xValue;
    int32_t curY = mappingInfo.yValue;
    if (rawMag > 0.0) {
        double dirX = rawZ / rawMag;
        double dirY = rawRZ / rawMag;
        double magnitude = (rawMag - DEAD_ZONE) / (1.0 - DEAD_ZONE);
        double curvedMag = std::pow(magnitude, 1.5);
        double initDelta = static_cast<double>(INIT_DELTA_US) / US_PER_SEC;
        curX += static_cast<int32_t>(dirX * curvedMag * static_cast<double>(baseSpeedX) * initDelta);
        curY += static_cast<int32_t>(dirY * curvedMag * static_cast<double>(baseSpeedY) * initDelta);
    }

    TouchEntity firstMove;
    firstMove.pointerId = pointerId_;
    firstMove.pointerAction = PointerEvent::POINTER_ACTION_MOVE;
    firstMove.xValue = curX;
    firstMove.yValue = curY;
    firstMove.actionTime = actionTime;
    BuildAndSendPointerEvent(context, firstMove);

    HILOGI("FPS started: anchor(%{public}d,%{public}d) speed(%{public}d,%{public}d)",
           mappingInfo.xValue, mappingInfo.yValue, baseSpeedX, baseSpeedY);

    task_.BindContext(context, mappingInfo, pointerId_, true, baseSpeedX, baseSpeedY);
    task_.StartTimer(StickObservationTask::FPS_INTERVAL_MS);
}

void ThumbStickFpsObservationToTouchHandler::DeactivateFpsObservation(
    std::shared_ptr<InputToTouchContext> &context,
    int64_t actionTime)
{
    task_.StopTimer();
    if (context->pointerItems.find(pointerId_) != context->pointerItems.end()) {
        PointerEvent::PointerItem lastItem = context->pointerItems[pointerId_];
        TouchEntity upEntity = BuildTouchUpEntity(lastItem, pointerId_,
                                                  PointerEvent::POINTER_ACTION_UP, actionTime);
        BuildAndSendPointerEvent(context, upEntity);
    }
    DelayedSingleton<PointerManager>::GetInstance()->ReleasePointerId(pointerId_);
    ResetState();
}

void ThumbStickFpsObservationToTouchHandler::HandleAxisEvent(
    std::shared_ptr<InputToTouchContext> &context,
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    const KeyToTouchMappingInfo &mappingInfo)
{
    int32_t action = pointerEvent->GetPointerAction();
    int64_t actionTime = pointerEvent->GetActionTime();

    if (action == PointerEvent::POINTER_ACTION_AXIS_END) {
        if (task_.IsActive()) {
            DeactivateFpsObservation(context, actionTime);
        }
        return;
    }

    // Read axis values
    PointerEvent::AxisType axisZ;
    PointerEvent::AxisType axisRZ;
    if (mappingInfo.joystick == STICK_LEFT) {
        axisZ = PointerEvent::AxisType::AXIS_TYPE_ABS_X;
        axisRZ = PointerEvent::AxisType::AXIS_TYPE_ABS_Y;
    } else {
        axisZ = PointerEvent::AxisType::AXIS_TYPE_ABS_Z;
        axisRZ = PointerEvent::AxisType::AXIS_TYPE_ABS_RZ;
    }
    if (!pointerEvent->HasAxis(axisZ) && !pointerEvent->HasAxis(axisRZ)) {
        return;
    }

    double rawZ = lastAxisZ_;
    double rawRZ = lastAxisRZ_;
    if (pointerEvent->HasAxis(axisZ)) {
        rawZ = pointerEvent->GetAxisValue(axisZ);
        lastAxisZ_ = rawZ;
    }
    if (pointerEvent->HasAxis(axisRZ)) {
        rawRZ = pointerEvent->GetAxisValue(axisRZ);
        lastAxisRZ_ = rawRZ;
    }

    double rawMag = std::sqrt(rawZ * rawZ + rawRZ * rawRZ);

    // Update task joystick data
    task_.UpdateJoystickData(rawZ, rawRZ);

    // Dead zone check
    if (rawMag < DEAD_ZONE) {
        if (task_.IsActive()) {
            DeactivateFpsObservation(context, actionTime);
        }
        return;
    }
    // Activate if not already running
    if (!task_.IsActive()) {
        ActivateFpsObservation(context, mappingInfo, actionTime, rawZ, rawRZ, rawMag);
    }
}

} // namespace GameController
} // namespace OHOS
