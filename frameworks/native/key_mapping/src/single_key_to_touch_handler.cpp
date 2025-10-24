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
    if (context->HasSingleKeyDown(keyCode)) {
        return;
    }

    HILOGI("keyCode [%{private}d] convert to down event of single-key-to-touch", keyCode);
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context->SetCurrentSingleKeyInfo(mappingInfo, pointerId);
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(mappingInfo, pointerId,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}

void SingleKeyToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                          const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                          const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (!context->HasSingleKeyDown(keyCode)) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. No single-key-operating",
               keyCode);
        return;
    }
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(keyCode);
    if (!pair.first) {
        HILOGW("discard keyCode [%{private}d]'s keyup event, because cannot find the pointerId", keyCode);
        return;
    }
    int32_t pointerId = pair.second;
    if (context->pointerItems.find(pointerId) == context->pointerItems.end()) {
        HILOGW("discard keyCode [%{private}d]'s keyup event, because cannot find the last point event", pointerId);
        return;
    }
    PointerEvent::PointerItem lastMovePoint = context->pointerItems[pointerId];

    HILOGI("keyCode [%{private}d] convert to up event of single-key-to-touch", keyCode);
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchUpEntity(lastMovePoint, SINGLE_POINT_ID,
                                                 PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->ResetCurrentSingleKeyInfo(keyCode);
}
}
}