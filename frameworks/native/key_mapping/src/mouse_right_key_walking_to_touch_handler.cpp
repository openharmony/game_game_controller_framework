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
#include "gamecontroller_utils.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t DELAY_TIME_UNIT = 1000000; // 1s = 1000ms = 1000000μs
const int32_t SLEEP_TIME = 40;
}

MouseRightKeyWalkingDelayHandleTask::MouseRightKeyWalkingDelayHandleTask()
{
    taskQueue_ = std::make_unique<ffrt::queue>("mouse-right-walking-thread",
                                               ffrt::queue_attr().qos(ffrt::qos_background));
}

MouseRightKeyWalkingDelayHandleTask::~MouseRightKeyWalkingDelayHandleTask()
{
}

void MouseRightKeyWalkingDelayHandleTask::StartDelayHandle(std::shared_ptr<InputToTouchContext> &context)
{
    std::lock_guard<ffrt::mutex> lock(taskLock_);
    int32_t delayTime = context->currentWalking.delayTime;
    if (delayTime <= 0) {
        SendUpEvent(context);
        return;
    }
    HILOGI("Start MouseRightKeyWalkingDelayHandleTask");
    hasDelayTask_ = true;
    context->isWalking = false;
    context->currentWalking = KeyToTouchMappingInfo();
    context_ = context;
    curTaskHandler_ = taskQueue_->submit_h([this] {
        DoDelayHandle();
    }, ffrt::task_attr().name("keyboard-observation-task").delay(delayTime * DELAY_TIME_UNIT));
}

bool MouseRightKeyWalkingDelayHandleTask::CancelDelayHandle()
{
    std::lock_guard<ffrt::mutex> lock(taskLock_);
    HILOGI("Cancel MouseRightKeyWalkingDelayHandleTask. hasDelayTask is [%{public}d]", hasDelayTask_ ? 1 : 0);
    if (hasDelayTask_) {
        hasDelayTask_ = false;
        taskQueue_->cancel(curTaskHandler_);
        SendUpEvent(context_);
        context_ = nullptr;
        return true;
    }
    context_ = nullptr;
    return false;
}

void MouseRightKeyWalkingDelayHandleTask::DoDelayHandle()
{
    std::lock_guard<ffrt::mutex> lock(taskLock_);
    HILOGI("Execute MouseRightKeyWalkingDelayHandleTask");
    if (!hasDelayTask_) {
        HILOGI("The MouseRightKeyWalkingDelayHandleTask has been canceled, so not send up event");
        return;
    }
    if (context_ == nullptr) {
        HILOGW("The MouseRightKeyWalkingDelayHandleTask's context_ is null, so not send up event");
        return;
    }
    hasDelayTask_ = false;
    SendUpEvent(context_);
    context_ = nullptr;
}

void MouseRightKeyWalkingDelayHandleTask::SendUpEvent(std::shared_ptr<InputToTouchContext> &context)
{
    HILOGI("Exit walking by mouse-right-key-walking");
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_WALK);
    if (!pair.first) {
        HILOGW("discard mouse-right up event. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    int64_t actionTime = StringUtils::GetSysClockTime();
    PointerEvent::PointerItem lastMovePoint = context->pointerItems[pointerId];
    TouchEntity touchEntity = BuildTouchUpEntity(lastMovePoint, pointerId,
                                                 PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->ResetCurrentWalking();
}

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
    DelayedSingleton<MouseRightKeyWalkingDelayHandleTask>::GetInstance()->CancelDelayHandle();
    context->SetCurrentWalking(mappingInfo, pointerId);
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(mappingInfo, pointerId,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);

    /**
     * 增加40ms的延迟,解决决胜巅峰中方向盘不固定时，由于第一个DOWN和MOVE间隔太短，
     * 导致游戏中的第一个手指按下的位置概率变为MOVE的坐标位置
     */
    ffrt::this_task::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
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
    DelayedSingleton<MouseRightKeyWalkingDelayHandleTask>::GetInstance()->StartDelayHandle(context);
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