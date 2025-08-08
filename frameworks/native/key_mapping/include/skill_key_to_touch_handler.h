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

#ifndef GAME_CONTROLLER_FRAMEWORK_SKILL_KEY_TO_TOUCH_HANDLER_H
#define GAME_CONTROLLER_FRAMEWORK_SKILL_KEY_TO_TOUCH_HANDLER_H

#include "key_to_touch_handler.h"

namespace OHOS {
namespace GameController {
class SkillKeyToTouchHandler : public BaseKeyToTouchHandler {
public:
    void HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                            const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                            const KeyToTouchMappingInfo &mappingInfo) override;

private:
    void HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                       const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                       const KeyToTouchMappingInfo &mappingInfo,
                       const DeviceInfo &deviceInfo) override;

    void HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                     const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                     const DeviceInfo &deviceInfo) override;

    bool IsNeedHandlePointEvent(std::shared_ptr<InputToTouchContext> &context,
                                const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    void HandleMouseMove(std::shared_ptr<InputToTouchContext> &context,
                         const std::shared_ptr<MMI::PointerEvent> &pointerEvent);
};
}
}
#endif //GAME_CONTROLLER_FRAMEWORK_SKILL_KEY_TO_TOUCH_HANDLER_H
