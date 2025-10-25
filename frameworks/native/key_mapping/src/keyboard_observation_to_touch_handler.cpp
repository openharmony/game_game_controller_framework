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

#include <cstdint>
#include "keyboard_observation_to_touch_handler.h"
#include "gamecontroller_log.h"
#include "gamecontroller_utils.h"

namespace OHOS {
namespace GameController {
namespace {
const std::string JOINER = "_";
const int32_t INVALID_VALUE = -1;
const int64_t FFRT_TASK_DELAY_TIME = 50000; // 50ms
}

KeyboardObservationToTouchHandler::KeyboardObservationToTouchHandler()
{
}

void KeyboardObservationToTouchHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                                      const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                      const KeyToTouchMappingInfo &mappingInfo,
                                                      const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (context->isPerspectiveObserving) {
        if (context->currentPerspectiveObserving.mappingType != KEY_BOARD_OBSERVATION_TO_TOUCH) {
            HILOGW("discard key[%{private}d] down event. mappingType is not keyboard_observation_to_touch", keyCode);
            return;
        }

        // calculate new position
        MoveByKeyDown(context, keyEvent, mappingInfo, deviceInfo);
        return;
    }

    HILOGI("keyCode [%{private}d] convert to down event of keyboard_observation_to_touch", keyCode);
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context->SetCurrentObserving(mappingInfo, pointerId);
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(context->currentPerspectiveObserving, pointerId,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);

    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->StartTaskByInterval();
    MoveByKeyDown(context, keyEvent, mappingInfo, deviceInfo);
}

void KeyboardObservationToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                                    const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                    const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (!context->isPerspectiveObserving) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. Not observing state.", keyCode);
        return;
    }
    if (context->currentPerspectiveObserving.mappingType != KEY_BOARD_OBSERVATION_TO_TOUCH) {
        HILOGW("discard key[%{private}d] up event. mappingType is not same with keyboard_observation_to_touch",
               keyCode);
        return;
    }
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_OBSERVATION);
    if (!pair.first) {
        HILOGW("discard keyup event. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;

    auto mapping = context->currentPerspectiveObserving;
    std::vector<DpadKeyItem> dpadKeys = CollectValidDpadKeys(keyEvent, deviceInfo, mapping);
    if (dpadKeys.empty()) {
        HILOGI("keyCode[%{private}d] convert to up event of keyboard_observation_to_touch", keyCode);
        DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->StopTask();
        int64_t actionTime = keyEvent->GetActionTime();
        TouchEntity touchEntity = BuildTouchEntity(mapping, pointerId,
                                                   PointerEvent::POINTER_ACTION_UP, actionTime);
        BuildAndSendPointerEvent(context, touchEntity);
        context->ResetCurrentObserving();
        return;
    } else {
        MoveByKeyUp(keyEvent, mapping, context, dpadKeys);
    }
}

void KeyboardObservationToTouchHandler::MoveByKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                                      const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                      const KeyToTouchMappingInfo &mappingInfo,
                                                      const DeviceInfo &deviceInfo)
{
    int32_t currentKeyCode = keyEvent->GetKeyCode();
    DpadKeyTypeEnum currentDpadKeyType = GetDpadKeyType(currentKeyCode, mappingInfo);
    if (currentDpadKeyType == DPAD_KEYTYPE_UNKNOWN) {
        HILOGW("the current keycode[%{private}d]'s keyType is unknown.", currentKeyCode);
        return;
    }

    std::vector<DpadKeyItem> dpadKeys = CollectValidDpadKeys(keyEvent, deviceInfo, mappingInfo);
    if (dpadKeys.empty()) {
        HILOGW("the current keycode[%{private}d]. no valid dpad keys.", currentKeyCode);
        return;
    }
    UpdateTaskInfo(context, currentKeyCode, currentDpadKeyType, dpadKeys);
}

void KeyboardObservationToTouchHandler::MoveByKeyUp(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                    const KeyToTouchMappingInfo &mappingInfo,
                                                    std::shared_ptr<InputToTouchContext> &context,
                                                    std::vector<DpadKeyItem> &dpadKeys)
{
    DpadKeyItem lastItem = dpadKeys.back();
    int32_t currentKeyCode = lastItem.keyCode;
    DpadKeyTypeEnum currentDpadKeyType = lastItem.keyTypeEnum;
    UpdateTaskInfo(context, currentKeyCode, currentDpadKeyType, dpadKeys);
}

void KeyboardObservationToTouchHandler::UpdateTaskInfo(std::shared_ptr<InputToTouchContext> &context,
                                                       int32_t currentKeyCode,
                                                       DpadKeyTypeEnum currentKeyType,
                                                       std::vector<DpadKeyItem> &dpadKeys)
{
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_OBSERVATION);
    if (!pair.first) {
        HILOGW("discard mouse move event, because cannot find pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    if (context->pointerItems.find(pointerId) == context->pointerItems.end()) {
        HILOGW("discard mouse move event, because cannot find the last move event");
        return;
    }
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->UpdateTaskInfo(currentKeyCode,
                                                                                           context,
                                                                                           currentKeyType,
                                                                                           dpadKeys);
}

KeyboardObservationToTouchHandlerTask::KeyboardObservationToTouchHandlerTask()
{
    taskQueue_ = std::make_unique<ffrt::queue>("keyboard-observation-thread",
                                               ffrt::queue_attr().qos(ffrt::qos_background));

    validCombinationKeys_.insert(std::to_string(DPAD_UP));
    validCombinationKeys_.insert(std::to_string(DPAD_UP) + JOINER + std::to_string(DPAD_LEFT));
    validCombinationKeys_.insert(std::to_string(DPAD_LEFT) + JOINER + std::to_string(DPAD_UP));
    validCombinationKeys_.insert(std::to_string(DPAD_UP) + JOINER + std::to_string(DPAD_RIGHT));
    validCombinationKeys_.insert(std::to_string(DPAD_RIGHT) + JOINER + std::to_string(DPAD_UP));
    validCombinationKeys_.insert(std::to_string(DPAD_DOWN));
    validCombinationKeys_.insert(std::to_string(DPAD_DOWN) + JOINER + std::to_string(DPAD_LEFT));
    validCombinationKeys_.insert(std::to_string(DPAD_LEFT) + JOINER + std::to_string(DPAD_DOWN));
    validCombinationKeys_.insert(std::to_string(DPAD_DOWN) + JOINER + std::to_string(DPAD_RIGHT));
    validCombinationKeys_.insert(std::to_string(DPAD_RIGHT) + JOINER + std::to_string(DPAD_DOWN));
    validCombinationKeys_.insert(std::to_string(DPAD_LEFT));
    validCombinationKeys_.insert(std::to_string(DPAD_RIGHT));
}

KeyboardObservationToTouchHandlerTask::~KeyboardObservationToTouchHandlerTask()
{
    HILOGI("KeyboardObservationToTouchHandlerTask::~KeyboardObservationToTouchHandlerTask success");
    StopTask();
}

void KeyboardObservationToTouchHandlerTask::StartTaskByInterval()
{
    std::lock_guard<ffrt::mutex> lock(taskLock_);
    if (taskIsStarting_) {
        HILOGE("start task failed. the task is running");
        return;
    }
    HILOGI("start KeyboardObservationToTouchHandlerTask success.");
    taskIsStarting_ = true;
    PutTaskToDelayQueue();
}

void KeyboardObservationToTouchHandlerTask::PutTaskToDelayQueue()
{
    curTaskHandler_ = taskQueue_->submit_h([this] {
        RunTask();
    }, ffrt::task_attr().name("keyboard-observation-task").delay(FFRT_TASK_DELAY_TIME));
}

void KeyboardObservationToTouchHandlerTask::StopTask()
{
    std::lock_guard<ffrt::mutex> lock(taskLock_);
    HILOGI("stop KeyboardObservationToTouchHandlerTask success.");
    taskIsStarting_ = false;
    context_ = nullptr;
    currentDpadKeyType_ = DPAD_KEYTYPE_UNKNOWN;
    currentKeyCode_ = 0;
    dpadKeys_.clear();
    taskQueue_->cancel(curTaskHandler_);
}

void KeyboardObservationToTouchHandlerTask::UpdateTaskInfo(int32_t currentKeyCode,
                                                           std::shared_ptr<InputToTouchContext> &context,
                                                           const DpadKeyTypeEnum currentKeyType,
                                                           std::vector<DpadKeyItem> &dpadKeys)
{
    if (context == nullptr || dpadKeys.empty() || currentKeyType == DPAD_KEYTYPE_UNKNOWN ||
        currentKeyCode == 0) {
        return;
    }
    std::lock_guard<ffrt::mutex> lock(taskLock_);
    currentKeyCode_ = currentKeyCode;
    context_ = context;
    currentDpadKeyType_ = currentKeyType;
    dpadKeys_ = dpadKeys;
}

void KeyboardObservationToTouchHandlerTask::RunTask()
{
    std::lock_guard<ffrt::mutex> lock(taskLock_);
    if (!taskIsStarting_) {
        HILOGE("the taskIsStarting_ is false, so not run the task.");
        return;
    }

    ComputeAndSendMovePointer();
    PutTaskToDelayQueue();
}

void KeyboardObservationToTouchHandlerTask::ComputeAndSendMovePointer()
{
    if (context_ == nullptr || dpadKeys_.empty()
        || currentDpadKeyType_ == DPAD_KEYTYPE_UNKNOWN || currentKeyCode_ == 0) {
        HILOGW("discard the compute event, because task info is invalid.");
        return;
    }
    std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_OBSERVATION);
    if (!pair.first) {
        HILOGW("discard compute. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    if (context_->pointerItems.find(pointerId) == context_->pointerItems.end()) {
        HILOGW("discard the compute event, because cannot find the last move event");
        return;
    }

    /**
     * When a combination key exists,
     * calculate the movement point using the current key and the first combination key.
     */
    std::string key;
    DpadKeyItem firstKeyItem;
    bool hasCombinationKeys = false;
    for (const auto &dpadKeyItem: dpadKeys_) {
        if (dpadKeyItem.keyCode == currentKeyCode_) {
            continue;
        }
        key = std::to_string(currentDpadKeyType_) + JOINER + std::to_string(dpadKeyItem.keyTypeEnum);
        if (validCombinationKeys_.find(key) != validCombinationKeys_.end()) {
            firstKeyItem = dpadKeyItem;
            hasCombinationKeys = true;
            break;
        }
    }
    PointerEvent::PointerItem lastMovePoint = context_->pointerItems[pointerId];
    Point targetPoint;
    targetPoint.x = lastMovePoint.GetWindowX();
    targetPoint.y = lastMovePoint.GetWindowY();
    if (hasCombinationKeys) {
        ComputeTargetPoint(context_, lastMovePoint, firstKeyItem.keyTypeEnum, targetPoint);
    }
    ComputeTargetPoint(context_, lastMovePoint, currentDpadKeyType_, targetPoint);
    int64_t actionTime = StringUtils::GetSysClockTime();
    TouchEntity touchEntity = BuildMoveTouchEntity(pointerId, targetPoint, actionTime);
    BuildAndSendPointerEvent(context_, touchEntity);
}

void KeyboardObservationToTouchHandlerTask::ComputeTargetPoint(std::shared_ptr<InputToTouchContext> &context,
                                                               const PointerEvent::PointerItem &lastMovePoint,
                                                               const DpadKeyTypeEnum currentKeyType,
                                                               Point &targetPoint)
{
    auto xStep = context->currentPerspectiveObserving.xStep;
    auto yStep = context->currentPerspectiveObserving.yStep;
    if (currentKeyType == DPAD_UP) {
        targetPoint.y = std::max(lastMovePoint.GetWindowY() - yStep, 0);
    } else if (currentKeyType == DPAD_DOWN) {
        targetPoint.y = std::min(lastMovePoint.GetWindowY() + yStep, context->windowInfoEntity.maxHeight);
    } else if (currentKeyType == DPAD_LEFT) {
        targetPoint.x = std::max(lastMovePoint.GetWindowX() - xStep, 0);
    } else {
        targetPoint.x = std::min(lastMovePoint.GetWindowX() + xStep, context->windowInfoEntity.maxWidth);
    }
}

}
}