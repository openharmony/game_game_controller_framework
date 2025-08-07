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

#ifndef GAME_CONTROLLER_FRAMEWORK_MOUSE_OBSERVATION_TO_TOUCH_HANDLER_H
#define GAME_CONTROLLER_FRAMEWORK_MOUSE_OBSERVATION_TO_TOUCH_HANDLER_H

#include "key_to_touch_handler.h"

namespace OHOS {
namespace GameController {
class MouseObservationToTouchHandler : public BaseKeyToTouchHandler {
public:
    virtual void HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                    const KeyToTouchMappingInfo &mappingInfo) override;

private:
    bool HandleMouseRightBtnDown(std::shared_ptr<InputToTouchContext> &context,
                                 const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                 const KeyToTouchMappingInfo &mappingInfo);

    void HandleMouseRightBtnUp(std::shared_ptr<InputToTouchContext> &context,
                               const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                               const KeyToTouchMappingInfo &mappingInfo);
};
}
}
#endif //GAME_CONTROLLER_FRAMEWORK_MOUSE_OBSERVATION_TO_TOUCH_HANDLER_H
