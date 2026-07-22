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
#include "stick_timer_manager.h"
#include "gamecontroller_log.h"
#include <algorithm>
#include <cmath>

namespace OHOS {
namespace GameController {
namespace {
constexpr int32_t TIMER_INTERVAL_MS = 8;
constexpr double DEAD_ZONE = 0.05;
constexpr double RESPONSE_EXPONENT = 1.5;
constexpr double IIR_ALPHA = 0.4;
constexpr int32_t DEFAULT_BASE_SPEED = 800;
constexpr double Y_AXIS_RATIO = 0.8;
constexpr int64_t INIT_DELTA_US = 8000;
constexpr double US_PER_SEC = 1000000.0;
constexpr int32_t DEAD_STOP_TICKS = 10;
constexpr double DEAD_ZONE_DECAY_FACTOR = 0.7;
constexpr int32_t EDGE_SAFE_MARGIN = 50;
}

ThumbStickFpsObservationToTouchHandler::~ThumbStickFpsObservationToTouchHandler()
{
    CancelTimer();
}

void ThumbStickFpsObservationToTouchHandler::ResetState()
{
    std::lock_guard<std::mutex> lock(stickMutex_);
    isActive_ = false;
    rawStickX_ = 0.0;
    rawStickY_ = 0.0;
    lastAxisZ_ = 0.0;
    lastAxisRZ_ = 0.0;
    filteredX_ = 0.0;
    filteredY_ = 0.0;
    if (needCenterFirst_) { return; }
    deadCounter_ = 0;
}

void ThumbStickFpsObservationToTouchHandler::CancelTimer()
{
    StickTimerManager::GetInstance()->Cancel(STICK_TIMER_FPS);
}

void ThumbStickFpsObservationToTouchHandler::HandleKeyDown(
    std::shared_ptr<InputToTouchContext> &, const std::shared_ptr<MMI::KeyEvent> &,
    const KeyToTouchMappingInfo &, const DeviceInfo &) {}
void ThumbStickFpsObservationToTouchHandler::HandleKeyUp(
    std::shared_ptr<InputToTouchContext> &, const std::shared_ptr<MMI::KeyEvent> &,
    const DeviceInfo &) {}

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

void ThumbStickFpsObservationToTouchHandler::StartFpsTimer(
    std::shared_ptr<InputToTouchContext> context, KeyToTouchMappingInfo mappingInfo)
{
    lastTick_ = std::chrono::steady_clock::now();
    StickTimerManager::GetInstance()->Start(STICK_TIMER_FPS,
        [this, context, mappingInfo]() { TimerTick(context, mappingInfo); }, TIMER_INTERVAL_MS);
}

void ThumbStickFpsObservationToTouchHandler::StopFpsTimer(
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

void ThumbStickFpsObservationToTouchHandler::HandleDeadZoneDecay(
    std::shared_ptr<InputToTouchContext> context)
{
    filteredX_ *= DEAD_ZONE_DECAY_FACTOR;
    filteredY_ *= DEAD_ZONE_DECAY_FACTOR;
    auto now = std::chrono::steady_clock::now();
    lastTick_ = now;
    TouchEntity moveEntity;
    moveEntity.pointerId = pointerId_;
    moveEntity.pointerAction = PointerEvent::POINTER_ACTION_MOVE;
    moveEntity.xValue = curX_;
    moveEntity.yValue = curY_;
    moveEntity.actionTime = 0;
    BuildAndSendPointerEvent(context, moveEntity);
}

void ThumbStickFpsObservationToTouchHandler::HandleEdgeReposition(
    std::shared_ptr<InputToTouchContext> context,
    const KeyToTouchMappingInfo &mappingInfo, int32_t pixelDx, int32_t pixelDy)
{
    if (curX_ < EDGE_SAFE_MARGIN || curX_ > maxW_ - EDGE_SAFE_MARGIN ||
        curY_ < EDGE_SAFE_MARGIN || curY_ > maxH_ - EDGE_SAFE_MARGIN) {
        PointerEvent::PointerItem lastItem;
        if (context->pointerItems.find(pointerId_) != context->pointerItems.end()) {
            lastItem = context->pointerItems[pointerId_];
        }
        TouchEntity upEntity = BuildTouchUpEntity(lastItem, pointerId_,
                                                  PointerEvent::POINTER_ACTION_UP, 0);
        BuildAndSendPointerEvent(context, upEntity);
        curX_ = anchorX_ + pixelDx;
        curY_ = anchorY_ + pixelDy;
        TouchEntity downEntity = BuildTouchEntity(mappingInfo, pointerId_,
                                                  PointerEvent::POINTER_ACTION_DOWN, 0);
        BuildAndSendPointerEvent(context, downEntity);
    }
}

void ThumbStickFpsObservationToTouchHandler::TimerTick(
    std::shared_ptr<InputToTouchContext> context, const KeyToTouchMappingInfo &mappingInfo)
{
    double rawX;
    double rawY;
    {
        std::lock_guard<std::mutex> lock(stickMutex_);
        rawX = rawStickX_;
        rawY = rawStickY_;
    }
    double rawMag = std::sqrt(rawX * rawX + rawY * rawY);
    if (needCenterFirst_) {
        needCenterFirst_ = false;
    }

    if (rawMag < DEAD_ZONE) {
        HandleDeadZoneDecay(context);
        return;
    }

    if (rawMag == 0.0) {
        return;
    }
    double dirX = rawX / rawMag;
    double dirY = rawY / rawMag;
    double magnitude = (rawMag - DEAD_ZONE) / (1.0 - DEAD_ZONE);
    double curvedMag = std::pow(magnitude, RESPONSE_EXPONENT);
    filteredX_ = IIR_ALPHA * dirX * curvedMag + (1.0 - IIR_ALPHA) * filteredX_;
    filteredY_ = IIR_ALPHA * dirY * curvedMag + (1.0 - IIR_ALPHA) * filteredY_;

    auto now = std::chrono::steady_clock::now();
    double deltaSec = std::chrono::duration<double>(now - lastTick_).count();
    lastTick_ = now;

    int32_t pixelDx = static_cast<int32_t>(filteredX_ * static_cast<double>(baseSpeedX_) * deltaSec);
    int32_t pixelDy = static_cast<int32_t>(filteredY_ * static_cast<double>(baseSpeedY_) * deltaSec);
    curX_ += pixelDx;
    curY_ += pixelDy;

    HandleEdgeReposition(context, mappingInfo, pixelDx, pixelDy);

    TouchEntity moveEntity;
    moveEntity.pointerId = pointerId_;
    moveEntity.pointerAction = PointerEvent::POINTER_ACTION_MOVE;
    moveEntity.xValue = curX_;
    moveEntity.yValue = curY_;
    moveEntity.actionTime = 0;
    BuildAndSendPointerEvent(context, moveEntity);
}

void ThumbStickFpsObservationToTouchHandler::GetStickAxisTypes(
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

bool ThumbStickFpsObservationToTouchHandler::ReadAndCacheStickAxes(
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    int32_t joystick, double &rawZ, double &rawRZ, double &rawMag)
{
    PointerEvent::AxisType axisZ;
    PointerEvent::AxisType axisRZ;
    GetStickAxisTypes(pointerEvent, joystick, axisZ, axisRZ);
    if (!pointerEvent->HasAxis(axisZ) && !pointerEvent->HasAxis(axisRZ)) {
        return false;
    }
    rawZ = lastAxisZ_;
    rawRZ = lastAxisRZ_;
    if (pointerEvent->HasAxis(axisZ)) {
        rawZ = pointerEvent->GetAxisValue(axisZ);
        lastAxisZ_ = rawZ;
    }
    if (pointerEvent->HasAxis(axisRZ)) {
        rawRZ = pointerEvent->GetAxisValue(axisRZ);
        lastAxisRZ_ = rawRZ;
    }
    rawMag = std::sqrt(rawZ * rawZ + rawRZ * rawRZ);
    {
        std::lock_guard<std::mutex> lock(stickMutex_);
        rawStickX_ = rawZ;
        rawStickY_ = rawRZ;
    }
    return true;
}

void ThumbStickFpsObservationToTouchHandler::ActivateFpsObservation(
    std::shared_ptr<InputToTouchContext> &context,
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    const KeyToTouchMappingInfo &mappingInfo,
    int64_t actionTime, double rawZ, double rawRZ, double rawMag)
{
    isActive_ = true;
    anchorX_ = mappingInfo.xValue;
    anchorY_ = mappingInfo.yValue;
    maxW_ = context->windowInfoEntity.maxWidth;
    maxH_ = context->windowInfoEntity.maxHeight;
    baseSpeedX_ = (mappingInfo.xStep > 0) ? mappingInfo.xStep : DEFAULT_BASE_SPEED;
    baseSpeedY_ = (mappingInfo.yStep > 0) ? mappingInfo.yStep
        : static_cast<int32_t>(baseSpeedX_ * Y_AXIS_RATIO);
    curX_ = anchorX_;
    curY_ = anchorY_;

    pointerId_ = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    TouchEntity downEntity = BuildTouchEntity(mappingInfo, pointerId_,
                                              PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, downEntity);

    if (rawMag == 0.0) {
        return;
    }
    double dirX = rawZ / rawMag;
    double dirY = rawRZ / rawMag;
    double magnitude = (rawMag - DEAD_ZONE) / (1.0 - DEAD_ZONE);
    double curvedMag = std::pow(magnitude, RESPONSE_EXPONENT);
    double initDelta = static_cast<double>(INIT_DELTA_US) / US_PER_SEC;
    curX_ += static_cast<int32_t>(dirX * curvedMag * static_cast<double>(baseSpeedX_) * initDelta);
    curY_ += static_cast<int32_t>(dirY * curvedMag * static_cast<double>(baseSpeedY_) * initDelta);

    TouchEntity firstMove;
    firstMove.pointerId = pointerId_;
    firstMove.pointerAction = PointerEvent::POINTER_ACTION_MOVE;
    firstMove.xValue = curX_;
    firstMove.yValue = curY_;
    firstMove.actionTime = actionTime;
    BuildAndSendPointerEvent(context, firstMove);

    HILOGI("FPS started: anchor(%{public}d,%{public}d) speed(%{public}d,%{public}d)",
           anchorX_, anchorY_, baseSpeedX_, baseSpeedY_);
    StartFpsTimer(context, mappingInfo);
}

void ThumbStickFpsObservationToTouchHandler::HandleAxisEvent(
    std::shared_ptr<InputToTouchContext> &context,
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    const KeyToTouchMappingInfo &mappingInfo)
{
    int32_t action = pointerEvent->GetPointerAction();
    int64_t actionTime = pointerEvent->GetActionTime();

    if (action == PointerEvent::POINTER_ACTION_AXIS_END) {
        if (StickTimerManager::GetInstance()->IsRunning(STICK_TIMER_FPS)) {
            StopFpsTimer(context, actionTime);
        }
        return;
    }

    double rawZ;
    double rawRZ;
    double rawMag;
    if (!ReadAndCacheStickAxes(pointerEvent, mappingInfo.joystick, rawZ, rawRZ, rawMag)) { return; }

    if (rawMag < DEAD_ZONE) {
        if (needCenterFirst_) { needCenterFirst_ = false; }
        deadCounter_++;
        if (deadCounter_ >= DEAD_STOP_TICKS
            && StickTimerManager::GetInstance()->IsRunning(STICK_TIMER_FPS)) {
            StopFpsTimer(context, actionTime);
            deadCounter_ = 0;
        }
        return;
    }
    if (needCenterFirst_) { needCenterFirst_ = false; }
    deadCounter_ = 0;

    if (!isActive_) {
        ActivateFpsObservation(context, pointerEvent, mappingInfo, actionTime, rawZ, rawRZ, rawMag);
    }
}

} // namespace GameController
} // namespace OHOS
