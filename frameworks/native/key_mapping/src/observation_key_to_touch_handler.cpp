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

#include "observation_key_to_touch_handler.h"

namespace OHOS {
namespace GameController {
void ObservationKeyToTouchHandler::HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                                      const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                      const KeyToTouchMappingInfo &mappingInfo)
{
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

void ObservationKeyToTouchHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                                 const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                 const KeyToTouchMappingInfo &mappingInfo,
                                                 const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (context->isPerspectiveObserving) {
        if (context->currentPerspectiveObserving.keyCode != keyCode) {
            HILOGW("discard keyCode [%{private}d]. It's perspectiveObserving now", keyCode);
        }
        return;
    }

    HILOGI("keyCode [%{private}d] convert to down event of observation_key_to_touch", keyCode);
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context->SetCurrentObserving(mappingInfo, pointerId);
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(context->currentPerspectiveObserving, pointerId,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}

void ObservationKeyToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                               const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                               const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (!context->isPerspectiveObserving) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. No observation-key-operating",
               keyCode);
        return;
    }
    if (context->currentPerspectiveObserving.keyCode != keyCode) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. keyCode is not current observation-key-operating keycode",
               keyCode);
        return;
    }
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_OBSERVATION);
    if (!pair.first) {
        HILOGW("discard keyup event. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    if (context->pointerItems.find(pointerId) != context->pointerItems.end()) {
        HILOGI("keyCode [%{private}d] convert to up event of observation-key-operating", keyCode);
        int64_t actionTime = keyEvent->GetActionTime();
        PointerEvent::PointerItem lastMovePoint = context->pointerItems[pointerId];
        TouchEntity touchEntity = BuildTouchUpEntity(lastMovePoint, pointerId,
                                                     PointerEvent::POINTER_ACTION_UP, actionTime);
        BuildAndSendPointerEvent(context, touchEntity);
    }

    context->ResetCurrentObserving();
}
}
}