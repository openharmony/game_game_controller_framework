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
#include "mouse_right_key_click_to_touch_handler.h"

namespace OHOS {
namespace GameController {
void MouseRightKeyClickToTouchHandler::HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                                          const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                          const KeyToTouchMappingInfo &mappingInfo)
{
    if (BaseKeyToTouchHandler::IsMouseRightButtonEvent(pointerEvent)) {
        if (HandleMouseRightBtnDown(context, pointerEvent, mappingInfo)) {
            return;
        }
        HandleMouseRightBtnUp(context, pointerEvent, mappingInfo);
    }
}

bool MouseRightKeyClickToTouchHandler::HandleMouseRightBtnDown(std::shared_ptr<InputToTouchContext> &context,
                                                               const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                               const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent->GetPointerAction() != PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        return false;
    }

    if (context->isSingleKeyOperating || context->isMouseLeftFireOperating) {
        HILOGW("discard mouse right-button up event. It's singleKeyOperating or mouseLeftFireOperating now");
        return true;
    }

    if (context->isMouseRightClickOperating) {
        return true;
    }
    HILOGI("convert to down event of mouse-right-click");
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(mappingInfo, SINGLE_POINT_ID,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->isMouseRightClickOperating = true;
    context->currentMouseRightClick = mappingInfo;
    return true;
}

void MouseRightKeyClickToTouchHandler::HandleMouseRightBtnUp(std::shared_ptr<InputToTouchContext> &context,
                                                             const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                             const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent->GetPointerAction() != PointerEvent::POINTER_ACTION_BUTTON_UP) {
        return;
    }

    if (!context->isMouseRightClickOperating) {
        HILOGW("discard mouse right-button up event. It's not mouseRightClickOperating now");
        return;
    }

    HILOGI("convert to up event of mouse-right-click");
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(mappingInfo, SINGLE_POINT_ID,
                                               PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->ResetMouseRightClick();
}
}
}