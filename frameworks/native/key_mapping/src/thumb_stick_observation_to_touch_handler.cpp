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
#include "stick_timer_manager.h"
#include "gamecontroller_log.h"
#include <algorithm>
#include <cmath>

namespace OHOS {
namespace GameController {
namespace {
constexpr int32_t TIMER_INTERVAL_MS = 50;
constexpr double DEAD_ZONE = 0.05;
constexpr int32_t DEFAULT_X_STEP = 200;
constexpr int32_t DEFAULT_Y_STEP = 200;
}

ThumbStickObservationToTouchHandler::~ThumbStickObservationToTouchHandler()
{
    CancelTimer();
}

void ThumbStickObservationToTouchHandler::ResetState()
{
    std::lock_guard<std::mutex> lock(stickMutex_);
    isActive_ = false;
    rawStickX_ = 0.0;
    rawStickY_ = 0.0;
    lastAxisZ_ = 0.0;
    lastAxisRZ_ = 0.0;
}

void ThumbStickObservationToTouchHandler::CancelTimer()
{
    StickTimerManager::GetInstance()->Cancel(STICK_TIMER_OBSERVATION);
}

void ThumbStickObservationToTouchHandler::HandleKeyDown(
    std::shared_ptr<InputToTouchContext> &, const std::shared_ptr<MMI::KeyEvent> &,
    const KeyToTouchMappingInfo &, const DeviceInfo &) {}
void ThumbStickObservationToTouchHandler::HandleKeyUp(
    std::shared_ptr<InputToTouchContext> &, const std::shared_ptr<MMI::KeyEvent> &,
    const DeviceInfo &) {}

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

void ThumbStickObservationToTouchHandler::StartObsTimer(
    std::shared_ptr<InputToTouchContext> context, KeyToTouchMappingInfo mappingInfo)
{
    StickTimerManager::GetInstance()->Start(STICK_TIMER_OBSERVATION,
        [this, context, mappingInfo]() { TimerTick(context, mappingInfo); }, TIMER_INTERVAL_MS);
}

void ThumbStickObservationToTouchHandler::StopObsTimer(
    std::shared_ptr<InputToTouchContext> &context, int64_t actionTime)
{
    CancelTimer();
    if (context->pointerItems.find(pointerId_) != context->pointerItems.end()) {
        PointerEvent::PointerItem lastItem = context->pointerItems[pointerId_];
        TouchEntity upEntity = BuildTouchUpEntity(lastItem, pointerId_,
                                                  PointerEvent::POINTER_ACTION_UP, actionTime);
        BuildAndSendPointerEvent(context, upEntity);
    }
    DelayedSingleton<PointerManager>::GetInstance()->ReleasePointerId(pointerId_);
    ResetState();
}

void ThumbStickObservationToTouchHandler::TimerTick(
    std::shared_ptr<InputToTouchContext> context, const KeyToTouchMappingInfo &mappingInfo)
{
    double stickX;
    double stickY;
    {
        std::lock_guard<std::mutex> lock(stickMutex_);
        stickX = rawStickX_;
        stickY = rawStickY_;
    }

    int32_t targetX = anchorX_ + static_cast<int32_t>(stickX * static_cast<double>(xStep_));
    int32_t targetY = anchorY_ + static_cast<int32_t>(stickY * static_cast<double>(yStep_));
    if (targetX >= 0 && targetX <= maxW_) {
        curX_ = targetX;
    }
    if (targetY >= 0 && targetY <= maxH_) {
        curY_ = targetY;
    }

    TouchEntity moveEntity;
    moveEntity.pointerId = pointerId_;
    moveEntity.pointerAction = PointerEvent::POINTER_ACTION_MOVE;
    moveEntity.xValue = curX_;
    moveEntity.yValue = curY_;
    moveEntity.actionTime = 0;
    BuildAndSendPointerEvent(context, moveEntity);
}

void ThumbStickObservationToTouchHandler::GetStickAxisTypes(
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    int32_t joystick, PointerEvent::AxisType &axisZ, PointerEvent::AxisType &axisRZ) const
{
    if (joystick == 0) {
        axisZ = PointerEvent::AxisType::AXIS_TYPE_ABS_X;
        axisRZ = PointerEvent::AxisType::AXIS_TYPE_ABS_Y;
    } else {
        axisZ = PointerEvent::AxisType::AXIS_TYPE_ABS_Z;
        axisRZ = PointerEvent::AxisType::AXIS_TYPE_ABS_RZ;
    }
}

void ThumbStickObservationToTouchHandler::ActivateObservation(
    std::shared_ptr<InputToTouchContext> &context,
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    const KeyToTouchMappingInfo &mappingInfo, int64_t actionTime)
{
    isActive_ = true;
    anchorX_ = mappingInfo.xValue;
    anchorY_ = mappingInfo.yValue;
    curX_ = anchorX_;
    curY_ = anchorY_;
    maxW_ = context->windowInfoEntity.maxWidth;
    maxH_ = context->windowInfoEntity.maxHeight;
    xStep_ = (mappingInfo.xStep > 0) ? mappingInfo.xStep : DEFAULT_X_STEP;
    yStep_ = (mappingInfo.yStep > 0) ? mappingInfo.yStep : DEFAULT_Y_STEP;
    pointerId_ = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    TouchEntity downEntity = BuildTouchEntity(mappingInfo, pointerId_,
                                              PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, downEntity);
    HILOGI("Observation started: anchor(%{public}d,%{public}d) step(%{public}d,%{public}d)",
           anchorX_, anchorY_, xStep_, yStep_);
    StartObsTimer(context, mappingInfo);
}

void ThumbStickObservationToTouchHandler::HandleAxisEvent(
    std::shared_ptr<InputToTouchContext> &context,
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    const KeyToTouchMappingInfo &mappingInfo)
{
    int32_t action = pointerEvent->GetPointerAction();
    int64_t actionTime = pointerEvent->GetActionTime();

    if (action == PointerEvent::POINTER_ACTION_AXIS_END) {
        if (StickTimerManager::GetInstance()->IsRunning(STICK_TIMER_OBSERVATION)) {
            StopObsTimer(context, actionTime);
        }
        return;
    }

    PointerEvent::AxisType axisZ;
    PointerEvent::AxisType axisRZ;
    GetStickAxisTypes(pointerEvent, mappingInfo.joystick, axisZ, axisRZ);
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
    {
        std::lock_guard<std::mutex> lock(stickMutex_);
        rawStickX_ = rawZ;
        rawStickY_ = rawRZ;
    }

    HILOGI("Observation: rawZ=%.3f rawRZ=%.3f mag=%.3f isActive=%{public}d",
           rawZ, rawRZ, rawMag, static_cast<int>(isActive_));
    if (rawMag < DEAD_ZONE) {
        if (needCenterFirst_) { needCenterFirst_ = false; }
        if (StickTimerManager::GetInstance()->IsRunning(STICK_TIMER_OBSERVATION)) {
            StopObsTimer(context, actionTime);
        }
        return;
    }
    if (needCenterFirst_) { return; }
    if (!isActive_) { ActivateObservation(context, pointerEvent, mappingInfo, actionTime); }
}

} // namespace GameController
} // namespace OHOS
