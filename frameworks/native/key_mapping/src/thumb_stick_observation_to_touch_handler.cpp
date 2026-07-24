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

#include "thumb_stick_observation_to_touch_handler.h"
#include "gamecontroller_log.h"
#include <algorithm>
#include <cmath>

namespace OHOS {
namespace GameController {
namespace {
constexpr double DEAD_ZONE = 0.05;
}

ThumbStickObservationToTouchHandler::~ThumbStickObservationToTouchHandler()
{
    CancelTimer();
}

void ThumbStickObservationToTouchHandler::ResetState()
{
    CancelTimer();
    lastAxisZ_ = 0.0;
    lastAxisRZ_ = 0.0;
}

void ThumbStickObservationToTouchHandler::CancelTimer()
{
    task_.StopTimer();
}

void ThumbStickObservationToTouchHandler::ReleaseIfActive(
    std::shared_ptr<InputToTouchContext> &context)
{
    if (task_.IsActive()) {
        DeactivateObservation(context, 0);
    } else {
        ResetState();
    }
}

void ThumbStickObservationToTouchHandler::SetNeedCenterFirst(bool value)
{
    needCenterFirst_ = value;
    task_.SetNeedCenterFirst(value);
}

void ThumbStickObservationToTouchHandler::HandlePointerEvent(
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

void ThumbStickObservationToTouchHandler::ActivateObservation(
    std::shared_ptr<InputToTouchContext> &context,
    const KeyToTouchMappingInfo &mappingInfo,
    int64_t actionTime)
{
    pointerId_ = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    TouchEntity downEntity = BuildTouchEntity(mappingInfo, pointerId_,
                                              PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, downEntity);
    HILOGI("Observation started: anchor(%{public}d,%{public}d) step(%{public}d,%{public}d)",
           mappingInfo.xValue, mappingInfo.yValue, mappingInfo.xStep, mappingInfo.yStep);
    task_.BindContext(context, mappingInfo, pointerId_, false, mappingInfo.xStep, mappingInfo.yStep);
    task_.StartTimer(StickObservationTask::OBSERVATION_INTERVAL_MS);
}

void ThumbStickObservationToTouchHandler::DeactivateObservation(
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

void ThumbStickObservationToTouchHandler::HandleAxisEvent(
    std::shared_ptr<InputToTouchContext> &context,
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    const KeyToTouchMappingInfo &mappingInfo)
{
    int32_t action = pointerEvent->GetPointerAction();
    int64_t actionTime = pointerEvent->GetActionTime();

    if (action == PointerEvent::POINTER_ACTION_AXIS_END) {
        if (task_.IsActive()) {
            DeactivateObservation(context, actionTime);
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
    HILOGI("Observation: rawZ=%.3f rawRZ=%.3f mag=%.3f isActive=%{public}d",
           rawZ, rawRZ, rawMag, static_cast<int>(task_.IsActive()));

    // Update task joystick data
    task_.UpdateJoystickData(rawZ, rawRZ);

    // Dead zone check
    if (rawMag < DEAD_ZONE) {
        if (needCenterFirst_) { needCenterFirst_ = false; }
        if (task_.IsActive()) {
            DeactivateObservation(context, actionTime);
        }
        return;
    }

    if (needCenterFirst_) { return; }

    // Activate if not already running
    if (!task_.IsActive()) {
        ActivateObservation(context, mappingInfo, actionTime);
    }
}

} // namespace GameController
} // namespace OHOS
