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

#include "combination_key_to_touch_handler.h"

namespace OHOS {
namespace GameController {

void CombinationKeyToTouchHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                                 const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                 const KeyToTouchMappingInfo &mappingInfo,
                                                 const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (mappingInfo.combinationKeys.size() != MAX_COMBINATION_KEYS) {
        HILOGW("discard keyCode [%{private}d], combinationKeys size is not equal to 2", keyCode);
        return;
    }

    if (context->isCombinationKeyOperating) {
        if (context->currentCombinationKey.combinationKeys.size() != MAX_COMBINATION_KEYS
            || context->currentCombinationKey.combinationKeys[COMBINATION_LAST_KEYCODE_IDX] != keyCode) {
            HILOGW("discard keyCode [%{private}d]. It's combination-operating now", keyCode);
        }
        return;
    }

    HILOGI("keyCode [%{private}d] convert to down event of combination-key-to-touch", keyCode);
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context->SetCurrentCombinationKey(mappingInfo, pointerId);
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(context->currentCombinationKey, pointerId,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}

void CombinationKeyToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                               const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                               const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (context->currentCombinationKey.combinationKeys.size() != MAX_COMBINATION_KEYS) {
        HILOGW("discard keyCode [%{private}d], combinationKeys size is not equal to 2", keyCode);
        return;
    }
    if (!context->isCombinationKeyOperating) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. No combination-operating",
               keyCode);
        return;
    }
    if (context->currentCombinationKey.combinationKeys[COMBINATION_LAST_KEYCODE_IDX] != keyCode) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. keyCode is not current combination-operating keycode",
               keyCode);
        return;
    }

    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_COMBINATION);
    if (!pair.first) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. because cannot find the pointerId", keyCode);
        return;
    }
    int32_t pointerId = pair.second;

    HILOGI("keyCode [%{private}d] convert to up event of combination-to-touch", keyCode);
    PointerEvent::PointerItem lastMovePoint = context->pointerItems[pointerId];
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchUpEntity(lastMovePoint, pointerId,
                                                 PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->ResetCurrentCombinationKey();
}
}
}