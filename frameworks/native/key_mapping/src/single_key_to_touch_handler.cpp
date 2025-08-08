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

#include "single_key_to_touch_handler.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
void SingleKeyToTouchHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                            const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                            const KeyToTouchMappingInfo &mappingInfo,
                                            const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (context->isMouseRightClickOperating || context->isMouseLeftFireOperating) {
        HILOGW("discard keyCode [%{private}d]. It's mouseRightClickOperating or mouseLeftFireOperating now", keyCode);
        return;
    }
    if (context->isSingleKeyOperating) {
        if (context->currentSingleKey.keyCode != keyCode) {
            HILOGW("discard keyCode [%{private}d]. It's single-key-operating now", keyCode);
        }
        return;
    }

    HILOGI("keyCode [%{private}d] convert to down event of single-key-to-touch", keyCode);
    context->isSingleKeyOperating = true;
    context->currentSingleKey = mappingInfo;
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(context->currentSingleKey, SINGLE_POINT_ID,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}

void SingleKeyToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                          const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                          const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (!context->isSingleKeyOperating) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. No single-key-operating",
               keyCode);
        return;
    }
    if (context->currentSingleKey.keyCode != keyCode) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. keyCode is not current single-key-operating keycode",
               keyCode);
        return;
    }
    if (context->pointerItems.find(SINGLE_POINT_ID) == context->pointerItems.end()) {
        HILOGW("discard button event, because cannot find the last point event");
        return;
    }
    PointerEvent::PointerItem lastMovePoint = context->pointerItems[SINGLE_POINT_ID];

    HILOGI("keyCode [%{private}d] convert to up event of single-key-to-touch", keyCode);
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchUpEntity(lastMovePoint, SINGLE_POINT_ID,
                                                 PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->ResetCurrentSingleKeyInfo();
}
}
}