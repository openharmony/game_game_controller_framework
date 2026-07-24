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

#ifndef GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_OBSERVATION_TO_TOUCH_HANDLER_H
#define GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_OBSERVATION_TO_TOUCH_HANDLER_H

#include "key_to_touch_handler.h"
#include "stick_observation_task.h"

namespace OHOS {
namespace GameController {
class ThumbStickObservationToTouchHandler : public BaseKeyToTouchHandler {
public:
    ThumbStickObservationToTouchHandler() = default;
    ~ThumbStickObservationToTouchHandler() override;

    void HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                            const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                            const KeyToTouchMappingInfo &mappingInfo) override;
    void ResetState();
    void SetNeedCenterFirst(bool value);
    void CancelTimer();
    void ReleaseIfActive(std::shared_ptr<InputToTouchContext> &context);

private:
    void HandleAxisEvent(std::shared_ptr<InputToTouchContext> &context,
                         const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                         const KeyToTouchMappingInfo &mappingInfo);
    void ActivateObservation(std::shared_ptr<InputToTouchContext> &context,
                             const KeyToTouchMappingInfo &mappingInfo,
                             int64_t actionTime);
    void DeactivateObservation(std::shared_ptr<InputToTouchContext> &context,
                               int64_t actionTime);

    StickObservationTask task_;
    int32_t pointerId_ = 0;
    double lastAxisZ_ = 0.0;
    double lastAxisRZ_ = 0.0;
    bool needCenterFirst_ = false;
};
}
}

#endif // GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_OBSERVATION_TO_TOUCH_HANDLER_H
