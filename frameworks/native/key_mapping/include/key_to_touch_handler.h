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
#ifndef GAME_CONTROLLER_FRAMEWORK_KEY_TO_TOUCH_HANDLER_H
#define GAME_CONTROLLER_FRAMEWORK_KEY_TO_TOUCH_HANDLER_H

#include <pointer_event.h>
#include <unordered_map>
#include <key_event.h>
#include "gamecontroller_keymapping_model.h"

namespace OHOS {
namespace GameController {
using namespace OHOS::MMI;

/**
 * the information of the current window
 */
struct WindowInfoEntity {
    int32_t windowId = 0;
    int32_t maxWidth = 0;
    int32_t maxHeight = 0;
    int32_t currentWidth = 0;
    int32_t currentHeight = 0;
    int32_t xPosition = 0;
    int32_t yPosition = 0;
    bool isFullScreen = false;
    int32_t xCenter = 0;
    int32_t yCenter = 0;
};

/**
 * the context for handle KeyEvent or PointerEvent
 */
struct InputToTouchContext {
    DeviceTypeEnum deviceType = UNKNOWN;
    std::unordered_map<int32_t, PointerEvent::PointerItem> pointerItems;
    bool isMonitorMouseMove = false;
    bool isCrosshairMode = false;
    std::unordered_map<int32_t, KeyToTouchMappingInfo> singleKeyMappings;
    std::unordered_map<int32_t, KeyToTouchMappingInfo> combinationKeyMappings;
    bool isSkillOperating = false;
    KeyToTouchMappingInfo currentSkillKeyInfo;
    bool isPerspectiveObserving = false;
    KeyToTouchMappingInfo currentPerspectiveObserving;
    PointerEvent::PointerItem lastMousePointer;
    WindowInfoEntity windowInfoEntity;
};

struct TouchEntity {
    int32_t pointerId = 0;
    int32_t pointerAction = 0;
    int32_t xValue;
    int32_t yValue;
    int32_t actionTime;
};

class BaseKeyToTouchHandler {
public:
    virtual void HandleKeyEvent(InputToTouchContext &context,
                                std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                const DeviceInfo &deviceInfo)
    {
    };

    virtual void HandlePointerEvent(InputToTouchContext &context,
                                    std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                    const DeviceInfo &deviceInfo)
    {
    };

protected:
    void BuildAndSendPointerEvent(InputToTouchContext &context, const TouchEntity &touchEntity);

private:
    PointerEvent::PointerItem BuildPointerItem(InputToTouchContext &context, const TouchEntity &touchEntity);
};

}
}
#endif //GAME_CONTROLLER_FRAMEWORK_KEY_TO_TOUCH_HANDLER_H
