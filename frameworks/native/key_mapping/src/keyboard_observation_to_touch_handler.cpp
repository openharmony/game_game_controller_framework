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

#include <cstdint>
#include "keyboard_observation_to_touch_handler.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
namespace {
const std::string JOINER = "_";
const int32_t INVALID_VALUE = -1;
}

KeyboardObservationToTouchHandler::KeyboardObservationToTouchHandler()
{
    validCombinationKeys_.insert(std::to_string(DPAD_UP));
    validCombinationKeys_.insert(std::to_string(DPAD_UP) + JOINER + std::to_string(DPAD_LEFT));
    validCombinationKeys_.insert(std::to_string(DPAD_LEFT) + JOINER + std::to_string(DPAD_UP));
    validCombinationKeys_.insert(std::to_string(DPAD_UP) + JOINER + std::to_string(DPAD_RIGHT));
    validCombinationKeys_.insert(std::to_string(DPAD_RIGHT) + JOINER + std::to_string(DPAD_UP));
    validCombinationKeys_.insert(std::to_string(DPAD_DOWN));
    validCombinationKeys_.insert(std::to_string(DPAD_DOWN) + JOINER + std::to_string(DPAD_LEFT));
    validCombinationKeys_.insert(std::to_string(DPAD_LEFT) + JOINER + std::to_string(DPAD_DOWN));
    validCombinationKeys_.insert(std::to_string(DPAD_DOWN) + JOINER + std::to_string(DPAD_RIGHT));
    validCombinationKeys_.insert(std::to_string(DPAD_RIGHT) + JOINER + std::to_string(DPAD_DOWN));
    validCombinationKeys_.insert(std::to_string(DPAD_LEFT));
    validCombinationKeys_.insert(std::to_string(DPAD_RIGHT));
}

void KeyboardObservationToTouchHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                                      const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                      const KeyToTouchMappingInfo &mappingInfo,
                                                      const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (context->isPerspectiveObserving) {
        if (context->currentPerspectiveObserving.mappingType != KEY_BOARD_OBSERVATION_TO_TOUCH) {
            HILOGW("discard key[%{private}d] down event. mappingType is not keyboard_observation_to_touch", keyCode);
            return;
        }

        // calculate new position
        MoveByKeyDown(context, keyEvent, mappingInfo, deviceInfo);
        return;
    }

    HILOGI("keyCode [%{private}d] convert to down event of keyboard_observation_to_touch", keyCode);
    context->isPerspectiveObserving = true;
    context->currentPerspectiveObserving = mappingInfo;
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(context->currentPerspectiveObserving, OBSERVATION_POINT_ID,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}

void KeyboardObservationToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                                    const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                    const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (!context->isPerspectiveObserving) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. Not observing state.", keyCode);
        return;
    }
    if (context->currentPerspectiveObserving.mappingType != KEY_BOARD_OBSERVATION_TO_TOUCH) {
        HILOGW("discard key[%{private}d] up event. mappingType is not same with keyboard_observation_to_touch",
               keyCode);
        return;
    }

    auto mapping = context->currentPerspectiveObserving;
    std::vector<DpadKeyItem> dpadKeys = CollectValidDpadKeys(keyEvent, deviceInfo, mapping);
    if (dpadKeys.empty()) {
        HILOGI("keyCode[%{private}d] convert to up event of keyboard_observation_to_touch", keyCode);
        int64_t actionTime = keyEvent->GetActionTime();
        TouchEntity touchEntity = BuildTouchEntity(mapping, OBSERVATION_POINT_ID,
                                                   PointerEvent::POINTER_ACTION_UP, actionTime);
        BuildAndSendPointerEvent(context, touchEntity);
        context->ResetCurrentPerspectiveObserving();
        return;
    } else {
        MoveByKeyUp(keyEvent, mapping, context, dpadKeys);
    }
}

void KeyboardObservationToTouchHandler::MoveByKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                                      const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                      const KeyToTouchMappingInfo &mappingInfo,
                                                      const DeviceInfo &deviceInfo)
{
    int32_t currentKeyCode = keyEvent->GetKeyCode();
    DpadKeyTypeEnum currentDpadKeyType = GetDpadKeyType(currentKeyCode, mappingInfo);
    if (currentDpadKeyType == DPAD_KEYTYPE_UNKNOWN) {
        HILOGW("the current keycode[%{private}d]'s keyType is unknown.", currentKeyCode);
        return;
    }

    std::vector<DpadKeyItem> dpadKeys = CollectValidDpadKeys(keyEvent, deviceInfo, mappingInfo);
    if (dpadKeys.empty()) {
        HILOGW("the current keycode[%{private}d]. no valid dpad keys.", currentKeyCode);
        return;
    }
    ContinueObserving(context, keyEvent, currentKeyCode, currentDpadKeyType, dpadKeys);
}

void KeyboardObservationToTouchHandler::MoveByKeyUp(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                    const KeyToTouchMappingInfo &mappingInfo,
                                                    std::shared_ptr<InputToTouchContext> &context,
                                                    std::vector<DpadKeyItem> &dpadKeys)
{
    DpadKeyItem lastItem = dpadKeys.back();
    int32_t currentKeyCode = lastItem.keyCode;
    DpadKeyTypeEnum currentDpadKeyType = lastItem.keyTypeEnum;
    ContinueObserving(context, keyEvent, currentKeyCode, currentDpadKeyType, dpadKeys);
}

void KeyboardObservationToTouchHandler::ComputeTargetPoint(std::shared_ptr<InputToTouchContext> &context,
                                                           const PointerEvent::PointerItem &lastMovePoint,
                                                           const DpadKeyTypeEnum currentKeyType,
                                                           Point &targetPoint)
{
    auto xStep = context->currentPerspectiveObserving.xStep;
    auto yStep = context->currentPerspectiveObserving.yStep;
    if (currentKeyType == DPAD_UP) {
        targetPoint.y = std::max(lastMovePoint.GetWindowY() - yStep, 0);
    } else if (currentKeyType == DPAD_DOWN) {
        targetPoint.y = std::min(lastMovePoint.GetWindowY() + yStep, context->windowInfoEntity.maxHeight);
    } else if (currentKeyType == DPAD_LEFT) {
        targetPoint.x = std::max(lastMovePoint.GetWindowX() - xStep, 0);
    } else {
        targetPoint.x = std::min(lastMovePoint.GetWindowX() + xStep, context->windowInfoEntity.maxWidth);
    }
}

void KeyboardObservationToTouchHandler::ContinueObserving(std::shared_ptr<InputToTouchContext> &context,
                                                          const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                          const int32_t currentKeyCode,
                                                          const DpadKeyTypeEnum currentKeyType,
                                                          std::vector<DpadKeyItem> &dpadKeys)
{
    if (context->pointerItems.find(OBSERVATION_POINT_ID) == context->pointerItems.end()) {
        HILOGW("discard mouse move event, because cannot find the last move event");
        return;
    }

    std::string key;
    DpadKeyItem firstKeyItem;
    bool hasCombinationKeys = false;
    for (const auto &dpadKeyItem: dpadKeys) {
        if (dpadKeyItem.keyCode == currentKeyCode) {
            continue;
        }
        key = std::to_string(currentKeyType) + JOINER + std::to_string(dpadKeyItem.keyTypeEnum);
        if (validCombinationKeys_.find(key) != validCombinationKeys_.end()) {
            firstKeyItem = dpadKeyItem;
            hasCombinationKeys = true;
            break;
        }
    }
    PointerEvent::PointerItem lastMovePoint = context->pointerItems[OBSERVATION_POINT_ID];
    Point targetPoint;
    targetPoint.x = lastMovePoint.GetWindowX();
    targetPoint.y = lastMovePoint.GetWindowY();
    if (hasCombinationKeys) {
        ComputeTargetPoint(context, lastMovePoint, firstKeyItem.keyTypeEnum, targetPoint);
    }
    ComputeTargetPoint(context, lastMovePoint, currentKeyType, targetPoint);
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildMoveTouchEntity(OBSERVATION_POINT_ID, targetPoint, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}
}
}