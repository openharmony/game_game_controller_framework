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

#ifndef GAME_CONTROLLER_FRAMEWORK_TRIGGER_TO_TOUCH_HANDLER_H
#define GAME_CONTROLLER_FRAMEWORK_TRIGGER_TO_TOUCH_HANDLER_H

#include "key_to_touch_handler.h"

#include "gamecontroller_client_model.h"

namespace OHOS {
namespace GameController {

/**
 * Virtual key codes for axis-to-key bridging (internal, not physical buttons).
 * Used by TriggerToTouchHandler to bridge analog trigger axes to key events.
 */
enum TriggerVirtualKey : int32_t {
    TRIGGER_VIRTUAL_LEFT_AXIS = 2500,
    TRIGGER_VIRTUAL_RIGHT_AXIS = 2501,
};

class TriggerToTouchHandler : public BaseKeyToTouchHandler {
public:
    TriggerToTouchHandler() = default;
    ~TriggerToTouchHandler() override = default;

    void HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                            const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                            const KeyToTouchMappingInfo &mappingInfo) override;

private:
    static constexpr double TRIGGER_THRESHOLD = 0.5;
    bool isActive_ = false;
};
}
}

#endif // GAME_CONTROLLER_FRAMEWORK_TRIGGER_TO_TOUCH_HANDLER_H
