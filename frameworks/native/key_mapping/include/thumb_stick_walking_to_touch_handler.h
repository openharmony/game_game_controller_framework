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

#ifndef GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_WALKING_TO_TOUCH_HANDLER_H
#define GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_WALKING_TO_TOUCH_HANDLER_H

#include "key_to_touch_handler.h"

namespace OHOS {
namespace GameController {
class ThumbStickWalkingToTouchHandler : public BaseKeyToTouchHandler {
public:
    ThumbStickWalkingToTouchHandler() = default;
    ~ThumbStickWalkingToTouchHandler() override = default;

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
    void ResetState()
    {
        isActive_ = false;
        lastAxisX_ = 0.0;
        lastAxisY_ = 0.0;
    }

private:
    void HandleAxisEvent(std::shared_ptr<InputToTouchContext> &context,
                         const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                         const KeyToTouchMappingInfo &mappingInfo);
    void GetStickAxisTypes(const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                           int32_t joystick,
                           PointerEvent::AxisType &axisX,
                           PointerEvent::AxisType &axisY) const;
    void UpdateStickAxes(const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                         const KeyToTouchMappingInfo &mappingInfo);
    void DeactivatePointer(std::shared_ptr<InputToTouchContext> &context, int64_t actionTime);

    static constexpr double DEAD_ZONE = 0.15;
    bool isActive_ = false;
    int32_t pointerId_ = 0;
    double lastAxisX_ = 0.0;
    double lastAxisY_ = 0.0;
};
}
}

#endif // GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_WALKING_TO_TOUCH_HANDLER_H
