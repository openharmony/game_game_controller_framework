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
#include "key_to_touch_handler.h"

namespace OHOS {
namespace GameController {
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

    void ContinueObserving(std::shared_ptr<InputToTouchContext> &context,
                           const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                           const int32_t currentKeyCode,
                           const DpadKeyTypeEnum currentKeyType,
                           std::vector<DpadKeyItem> &dpadKeys);

    void ComputeTargetPoint(std::shared_ptr<InputToTouchContext> &context,
                            const PointerEvent::PointerItem &lastMovePoint,
                            const DpadKeyTypeEnum currentKeyType,
                            Point &targetPoint);

private:
    std::unordered_set<std::string> validCombinationKeys_;
};
}
}

#endif //GAME_CONTROLLER_FRAMEWORK_SINGLE_KEYBOARD_OBSERVATION_TO_TOUCH_HANDLER_H
