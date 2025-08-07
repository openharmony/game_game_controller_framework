/*
 *  Copyright (c) 2025 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <input_manager.h>
#include "crosshair_key_to_touch_handler.h"

namespace OHOS {
namespace GameController {

void CrosshairKeyToTouchHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                               const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                               const KeyToTouchMappingInfo &mappingInfo,
                                               const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (context->isCrosshairMode) {
        if (context->currentCrosshairInfo.keyCode != keyCode) {
            HILOGW("discard keyCode [%{private}d]. It's cross-key-operating now", keyCode);
        }
        return;
    }

    HILOGI("enter into CrosshairMode");
    context->isCrosshairMode = true;
    context->isEnterCrosshairInfo = false;
    context->currentCrosshairInfo = mappingInfo;

    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(context->currentCrosshairInfo, CROSSHAIR_POINT_ID,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}

void CrosshairKeyToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                             const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                             const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (!context->isCrosshairMode) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. No crosshair-key-operating",
               keyCode);
        return;
    }

    if (context->isEnterCrosshairInfo) {
        // exit the crosshair mode
        if (context->pointerItems.find(CROSSHAIR_POINT_ID) != context->pointerItems.end()) {
            int64_t actionTime = keyEvent->GetActionTime();
            PointerEvent::PointerItem lastMovePoint = context->pointerItems[CROSSHAIR_POINT_ID];
            TouchEntity touchEntity = BuildTouchUpEntity(lastMovePoint, CROSSHAIR_POINT_ID,
                                                         PointerEvent::POINTER_ACTION_UP, actionTime);
            BuildAndSendPointerEvent(context, touchEntity);
        }
        HILOGI("exit CrosshairMode, and show mouse pointer");
        InputManager::GetInstance()->SetPointerVisible(true, 0);
        context->ResetCrosshairInfo();
        return;
    }

    context->isEnterCrosshairInfo = true;
    HILOGI("keyCode [%{private}d] hide mouse pointer", keyCode);
    InputManager::GetInstance()->SetPointerVisible(false, 0);
}

void CrosshairKeyToTouchHandler::HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                                    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                    const KeyToTouchMappingInfo &mappingInfo)
{
    if (!BaseKeyToTouchHandler::IsMouseMoveEvent(pointerEvent)) {
        return;
    }

    if (!context->isEnterCrosshairInfo) {
        return;
    }

    // send move event
    ComputeTouchPointByMouseMoveEvent(context, pointerEvent, mappingInfo, CROSSHAIR_POINT_ID);
    if (context->pointerItems.find(CROSSHAIR_POINT_ID) == context->pointerItems.end()) {
        HILOGW("discard mouse move event, because cannot find the last move event");
        return;
    }

    PointerEvent::PointerItem lastMovePoint = context->pointerItems[CROSSHAIR_POINT_ID];
    if (lastMovePoint.GetWindowX() == context->windowInfoEntity.maxWidth
        || lastMovePoint.GetWindowX() <= MIN_EDGE) {
        /*
         * if move to x's edge, need to send the up and down touch event, reset the touch's start point.
         * achieve 360-degree rotation
         */
        int64_t actionTime = pointerEvent->GetActionTime();
        TouchEntity touchEntity = BuildTouchUpEntity(lastMovePoint, CROSSHAIR_POINT_ID,
                                                     PointerEvent::POINTER_ACTION_UP, actionTime);
        BuildAndSendPointerEvent(context, touchEntity);

        touchEntity = BuildTouchEntity(context->currentCrosshairInfo, CROSSHAIR_POINT_ID,
                                       PointerEvent::POINTER_ACTION_DOWN, actionTime);
        BuildAndSendPointerEvent(context, touchEntity);
    }
}
}
}