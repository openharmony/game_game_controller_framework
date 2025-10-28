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

#include "mouse_observation_to_touch_handler.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {

void MouseObservationToTouchHandler::HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                                        const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                        const KeyToTouchMappingInfo &mappingInfo)
{
    if (BaseKeyToTouchHandler::IsMouseRightButtonEvent(pointerEvent)) {
        if (HandleMouseRightBtnDown(context, pointerEvent, mappingInfo)) {
            return;
        }
        HandleMouseRightBtnUp(context, pointerEvent, mappingInfo);
        return;
    }
    if (BaseKeyToTouchHandler::IsMouseMoveEvent(pointerEvent)) {
        std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_OBSERVATION);
        if (!pair.first) {
            HILOGW("discard mouse move event. because cannot find the pointerId");
            return;
        }
        int32_t pointerId = pair.second;
        ComputeTouchPointByMouseMoveEvent(context, pointerEvent, mappingInfo, pointerId);
    }
}

bool MouseObservationToTouchHandler::HandleMouseRightBtnDown(std::shared_ptr<InputToTouchContext> &context,
                                                             const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                             const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent->GetPointerAction() != PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        return false;
    }

    if (context->isPerspectiveObserving) {
        HILOGW("discard mouse right-button down event. It's perspectiveObserving now");
        return true;
    }

    HILOGI("convert to down event of mouse-observation");
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context->SetCurrentObserving(mappingInfo, pointerId);
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(mappingInfo, pointerId,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    PointerEvent::PointerItem pointerItem;
    pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem);
    context->lastMousePointer = pointerItem;
    return true;
}

void MouseObservationToTouchHandler::HandleMouseRightBtnUp(std::shared_ptr<InputToTouchContext> &context,
                                                           const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                           const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent->GetPointerAction() != PointerEvent::POINTER_ACTION_BUTTON_UP) {
        return;
    }
    if (!context->isPerspectiveObserving) {
        HILOGW("discard mouse right-button up event. It's not perspectiveObserving");
        return;
    }
    if (context->currentPerspectiveObserving.mappingType != MOUSE_OBSERVATION_TO_TOUCH) {
        HILOGW("discard mouse right-button up event. mappingType is not same with mouse_observation_to_touch");
        return;
    }
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_OBSERVATION);
    if (!pair.first) {
        HILOGW("discard mouse right-button up event. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(mappingInfo, pointerId,
                                               PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->ResetCurrentObserving();
}
}
}