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
#include <mutex>

namespace OHOS {
namespace GameController {
class ThumbStickObservationToTouchHandler : public BaseKeyToTouchHandler {
public:
    ThumbStickObservationToTouchHandler() = default;
    ~ThumbStickObservationToTouchHandler() override;

    void HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                       const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                       const KeyToTouchMappingInfo &mappingInfo,
                       const DeviceInfo &deviceInfo) override;
    void HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                     const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                     const DeviceInfo &deviceInfo) override;
    void HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                            const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                            const KeyToTouchMappingInfo &mappingInfo) override;
    void ResetState();
    void SetNeedCenterFirst(bool value) { needCenterFirst_ = value; }
    void CancelTimer();

private:
    void HandleAxisEvent(std::shared_ptr<InputToTouchContext> &context,
                         const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                         const KeyToTouchMappingInfo &mappingInfo);
    void GetStickAxisTypes(const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                           int32_t joystick, PointerEvent::AxisType &axisZ,
                           PointerEvent::AxisType &axisRZ) const;
    void ActivateObservation(std::shared_ptr<InputToTouchContext> &context,
                             const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                             const KeyToTouchMappingInfo &mappingInfo, int64_t actionTime);
    void StartObsTimer(std::shared_ptr<InputToTouchContext> context,
                       KeyToTouchMappingInfo mappingInfo);
    void StopObsTimer(std::shared_ptr<InputToTouchContext> &context, int64_t actionTime);
    void TimerTick(std::shared_ptr<InputToTouchContext> context,
                   const KeyToTouchMappingInfo &mappingInfo);

    std::mutex stickMutex_;
    bool isActive_ = false;
    int32_t pointerId_ = 0;
    bool needCenterFirst_ = false;
    double rawStickX_ = 0.0;
    double rawStickY_ = 0.0;
    double lastAxisZ_ = 0.0;
    double lastAxisRZ_ = 0.0;
    int32_t curX_ = 0;
    int32_t curY_ = 0;
    int32_t anchorX_ = 0;
    int32_t anchorY_ = 0;
    int32_t xStep_ = 0;
    int32_t yStep_ = 0;
    int32_t maxW_ = 0;
    int32_t maxH_ = 0;
};
}
}

#endif // GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_OBSERVATION_TO_TOUCH_HANDLER_H