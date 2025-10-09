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

#ifndef GAME_CONTROLLER_FRAMEWORK_SINGLE_KEYBOARD_OBSERVATION_TO_TOUCH_HANDLER_H
#define GAME_CONTROLLER_FRAMEWORK_SINGLE_KEYBOARD_OBSERVATION_TO_TOUCH_HANDLER_H

#include <mutex>
#include <singleton.h>
#include <cpp/queue.h>
#include "key_to_touch_handler.h"
#include "ffrt.h"

namespace OHOS {
namespace GameController {
class KeyboardObservationToTouchHandlerTask
    : public DelayedSingleton<KeyboardObservationToTouchHandlerTask>, BaseKeyToTouchHandler {
DECLARE_DELAYED_SINGLETON(KeyboardObservationToTouchHandlerTask)

public:
    /**
     * Start task to send the move pointer
     */
    void StartTaskByInterval();

    /**
     * Stop task
     */
    void StopTask();

    /**
     * Update the information of task
     * @param currentKeyCode current pressed keycode
     * @param context InputToTouchContext
     * @param currentKeyType DpadKeyTypeEnum
     * @param dpadKeys the current pressed keys information of dpad
     */
    void UpdateTaskInfo(int32_t currentKeyCode,
                        std::shared_ptr<InputToTouchContext> &context,
                        const DpadKeyTypeEnum currentKeyType,
                        std::vector<DpadKeyItem> &dpadKeys);

private:
    void PutTaskToDelayQueue();

    /**
     * Run the task
     */
    void RunTask();

    bool IsValidTask();

    /**
     * Compute and send the move pointer.
     */
    void ComputeAndSendMovePointer();

    /**
     * Compute the target point
     * @param context InputToTouchContext
     * @param lastMovePoint the last move point
     * @param currentKeyType current pressed keyType
     * @param targetPoint the target point
     */
    void ComputeTargetPoint(std::shared_ptr<InputToTouchContext> &context,
                            const PointerEvent::PointerItem &lastMovePoint,
                            const DpadKeyTypeEnum currentKeyType,
                            Point &targetPoint);

private:
    ffrt::mutex taskLock_;
    std::unique_ptr<ffrt::queue> taskQueue_{nullptr};
    ffrt::task_handle curTaskHandler_;
    bool taskIsStarting_{false};
    std::shared_ptr<InputToTouchContext> context_{nullptr};
    std::vector<DpadKeyItem> dpadKeys_;
    DpadKeyTypeEnum currentDpadKeyType_{DPAD_KEYTYPE_UNKNOWN};
    int32_t currentKeyCode_{0};
    std::unordered_set<std::string> validCombinationKeys_;
};

class KeyboardObservationToTouchHandler : public BaseKeyToTouchHandler {
public:
    KeyboardObservationToTouchHandler();

protected:
    void HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                       const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                       const KeyToTouchMappingInfo &mappingInfo,
                       const DeviceInfo &deviceInfo) override;

    void HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                     const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                     const DeviceInfo &deviceInfo) override;

private:
    void MoveByKeyDown(std::shared_ptr<InputToTouchContext> &context,
                       const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                       const KeyToTouchMappingInfo &mappingInfo,
                       const DeviceInfo &deviceInfo);

    void MoveByKeyUp(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                     const KeyToTouchMappingInfo &mappingInfo,
                     std::shared_ptr<InputToTouchContext> &context,
                     std::vector<DpadKeyItem> &dpadKeys);

    void UpdateTaskInfo(std::shared_ptr<InputToTouchContext> &context,
                        int32_t currentKeyCode,
                        DpadKeyTypeEnum currentKeyType,
                        std::vector<DpadKeyItem> &dpadKeys);
};
}
}

#endif //GAME_CONTROLLER_FRAMEWORK_SINGLE_KEYBOARD_OBSERVATION_TO_TOUCH_HANDLER_H
