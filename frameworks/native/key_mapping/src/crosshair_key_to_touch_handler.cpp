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
namespace {
const int32_t ONE_LOOP = 10;
}

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
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context->SetCurrentCrosshairInfo(mappingInfo, pointerId);
    SendDownTouch(context, keyEvent->GetActionTime());
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
        SendUpTouch(context, keyEvent->GetActionTime());
        HILOGI("exit CrosshairMode, and show mouse pointer");
        ExitCrosshairKeyStatus();
        context->ResetCurrentCrosshairInfo();
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

    if (currentIdx_ == 0) {
        SendDownTouch(context, pointerEvent->GetActionTime());
        SetLastMousePoint(context, pointerEvent);
    } else if (currentIdx_ == ONE_LOOP) {
        SendUpTouch(context, pointerEvent->GetActionTime());
        SetLastMousePoint(context, pointerEvent);
    } else {
        SendMoveTouch(context, pointerEvent, mappingInfo);
    }
}

void CrosshairKeyToTouchHandler::SendMoveTouch(std::shared_ptr<InputToTouchContext> &context,
                                               const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                               const KeyToTouchMappingInfo &mappingInfo)
{
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_CROSSHAIR);
    if (!pair.first) {
        HILOGW("discard send move touch. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    currentIdx_++;
    ComputeTouchPointByMouseMoveEvent(context, pointerEvent, mappingInfo, pointerId);
}

void CrosshairKeyToTouchHandler::SendDownTouch(std::shared_ptr<InputToTouchContext> &context, int64_t actionTime)
{
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_CROSSHAIR);
    if (!pair.first) {
        HILOGW("discard send down touch. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    TouchEntity touchEntity = BuildTouchEntity(context->currentCrosshairInfo, pointerId,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    currentIdx_ = 1;
}

void CrosshairKeyToTouchHandler::SendUpTouch(std::shared_ptr<InputToTouchContext> &context, int64_t actionTime)
{
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_CROSSHAIR);
    if (!pair.first) {
        HILOGW("discard send up touch. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    if (context->pointerItems.find(pointerId) != context->pointerItems.end()) {
        PointerEvent::PointerItem lastMovePoint = context->pointerItems[pointerId];
        TouchEntity touchEntity = BuildTouchUpEntity(lastMovePoint, pointerId,
                                                     PointerEvent::POINTER_ACTION_UP, actionTime);
        BuildAndSendPointerEvent(context, touchEntity);
    }
    currentIdx_ = 0;
}

void CrosshairKeyToTouchHandler::ExitCrosshairKeyStatus()
{
    InputManager::GetInstance()->SetPointerVisible(true, 0);
    currentIdx_ = 0;
}

void CrosshairKeyToTouchHandler::SetLastMousePoint(std::shared_ptr<InputToTouchContext> &context,
                                                   const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    PointerEvent::PointerItem currentPointItem;
    pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), currentPointItem);
    context->lastMousePointer = currentPointItem;
}

}
}