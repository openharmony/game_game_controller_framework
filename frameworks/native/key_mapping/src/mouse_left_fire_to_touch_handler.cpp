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

#include "mouse_left_fire_to_touch_handler.h"

namespace OHOS {
namespace GameController {
void MouseLeftFireToTouchHandler::HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                                     const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                     const KeyToTouchMappingInfo &mappingInfo)
{
    if (BaseKeyToTouchHandler::IsMouseLeftButtonEvent(pointerEvent)) {
        if (HandleMouseLeftBtnDown(context, pointerEvent, mappingInfo)) {
            return;
        }
        HandleMouseLeftBtnUp(context, pointerEvent, mappingInfo);
    }
}

bool MouseLeftFireToTouchHandler::HandleMouseLeftBtnDown(std::shared_ptr<InputToTouchContext> &context,
                                                         const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                         const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent->GetPointerAction() != PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        return false;
    }

    if (context->isSingleKeyOperating || context->isMouseRightClickOperating) {
        HILOGW("discard mouse left-button down event. It's singleKeyOperating or mouseRightClickOperating now");
        return true;
    }

    if (context->isMouseLeftFireOperating || !context->isEnterCrosshairInfo) {
        return true;
    }

    HILOGI("convert to down event of mouse-left-fire");
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(mappingInfo, SINGLE_POINT_ID,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->isMouseLeftFireOperating = true;
    return true;
}

void MouseLeftFireToTouchHandler::HandleMouseLeftBtnUp(std::shared_ptr<InputToTouchContext> &context,
                                                       const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                       const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent->GetPointerAction() != PointerEvent::POINTER_ACTION_BUTTON_UP) {
        return;
    }

    if (!context->isMouseLeftFireOperating) {
        HILOGW("discard mouse left-button up event. It's not mouseLeftFireOperating now");
        return;
    }

    HILOGI("convert to up event of mouse-left-fire");
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(mappingInfo, SINGLE_POINT_ID,
                                               PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->isMouseLeftFireOperating = false;
}
}
}