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
#include <algorithm>
#include <cstdint>
#include "dpad_key_to_touch_handler.h"
#include "ffrt.h"

namespace OHOS {
namespace GameController {
namespace {
const std::string JOINER = "_";
const int32_t INVALID_VALUE = -1;
const int32_t UP_ANGLE = 270;
const int32_t DOWN_ANGLE = 90;
const int32_t LEFT_ANGLE = 180;
const int32_t RIGHT_ANGLE = 0;
const int32_t UP_LEFT_ANGLE = 225;
const int32_t UP_RIGHT_ANGLE = 315;
const int32_t DOWN_LEFT_ANGLE = 135;
const int32_t DOWN_RIGHT_ANGLE = 45;
const int32_t SLEEP_TIME = 40;
}

DpadKeyToTouchHandler::DpadKeyToTouchHandler()
{
    keyAngleMap_[std::to_string(DPAD_UP)] = UP_ANGLE;
    keyAngleMap_[std::to_string(DPAD_UP) + JOINER + std::to_string(DPAD_LEFT)] = UP_LEFT_ANGLE;
    keyAngleMap_[std::to_string(DPAD_LEFT) + JOINER + std::to_string(DPAD_UP)] = UP_LEFT_ANGLE;
    keyAngleMap_[std::to_string(DPAD_UP) + JOINER + std::to_string(DPAD_RIGHT)] = UP_RIGHT_ANGLE;
    keyAngleMap_[std::to_string(DPAD_RIGHT) + JOINER + std::to_string(DPAD_UP)] = UP_RIGHT_ANGLE;
    keyAngleMap_[std::to_string(DPAD_DOWN)] = DOWN_ANGLE;
    keyAngleMap_[std::to_string(DPAD_DOWN) + JOINER + std::to_string(DPAD_LEFT)] = DOWN_LEFT_ANGLE;
    keyAngleMap_[std::to_string(DPAD_LEFT) + JOINER + std::to_string(DPAD_DOWN)] = DOWN_LEFT_ANGLE;
    keyAngleMap_[std::to_string(DPAD_DOWN) + JOINER + std::to_string(DPAD_RIGHT)] = DOWN_RIGHT_ANGLE;
    keyAngleMap_[std::to_string(DPAD_RIGHT) + JOINER + std::to_string(DPAD_DOWN)] = DOWN_RIGHT_ANGLE;
    keyAngleMap_[std::to_string(DPAD_LEFT)] = LEFT_ANGLE;
    keyAngleMap_[std::to_string(DPAD_RIGHT)] = RIGHT_ANGLE;
}

void DpadKeyToTouchHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                          const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                          const KeyToTouchMappingInfo &mappingInfo,
                                          const DeviceInfo &deviceInfo)
{
    if (context->isWalking) {
        if (context->currentWalking.mappingType == DPAD_KEY_TO_TOUCH) {
            MoveByKeyDown(context, keyEvent, mappingInfo, deviceInfo);
        }
        return;
    }
    HILOGI("Enter walking by dpad");
    context->isWalking = true;
    context->currentWalking = mappingInfo;
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(mappingInfo, WALK_POINT_ID,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);

    /**
     * 增加40ms的延迟,解决决胜巅峰中方向盘不固定时，由于第一个DOWN和MOVE间隔太短，
     * 导致游戏中的第一个手指按下的位置概率变为MOVE的坐标位置
     */
    ffrt::this_task::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
    MoveByKeyDown(context, keyEvent, mappingInfo, deviceInfo);
}

void DpadKeyToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                        const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                        const DeviceInfo &deviceInfo)
{
    if (!context->isWalking
        || context->currentWalking.mappingType != DPAD_KEY_TO_TOUCH) {
        return;
    }

    std::vector<DpadKeyItem> dpadKeys = CollectValidDpadKeys(keyEvent, deviceInfo, context->currentWalking);
    if (!dpadKeys.empty()) {
        //If there are pressed dpad keys, calculate the movement position based on the first and last pressed buttons.
        MoveByKeyUp(keyEvent, context->currentWalking, context, dpadKeys);
        return;
    }

    // If no directional key is pressed, need to exit walking.
    HILOGI("Exit walking by dpad");
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(context->currentWalking, WALK_POINT_ID,
                                               PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->ResetCurrentWalking();
}

void DpadKeyToTouchHandler::MoveByKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                          const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                          const KeyToTouchMappingInfo &mappingInfo,
                                          const DeviceInfo &deviceInfo)
{
    // Get the pressed dpad's keycode
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
    int32_t angle = GetAngle(currentKeyCode, currentDpadKeyType, dpadKeys);
    if (angle == INVALID_VALUE) {
        HILOGW("cannot find valid angle value. the keycode is [%{private}d]'s", currentKeyCode);
        return;
    }
    MoveToTarget(context, keyEvent, mappingInfo, angle);
}

void DpadKeyToTouchHandler::MoveToTarget(std::shared_ptr<InputToTouchContext> &context,
                                         const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                         const KeyToTouchMappingInfo &mappingInfo, const int32_t angle)
{
    Point centerPoint;
    centerPoint.x = mappingInfo.xValue;
    centerPoint.y = mappingInfo.yValue;
    Point targetPoint = ComputeTargetPoint(centerPoint, mappingInfo.radius, static_cast<double>(angle));
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildMoveTouchEntity(WALK_POINT_ID, targetPoint, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}

void DpadKeyToTouchHandler::MoveByKeyUp(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                        const KeyToTouchMappingInfo &mappingInfo,
                                        std::shared_ptr<InputToTouchContext> &context,
                                        std::vector<DpadKeyItem> &dpadKeys)
{
    // compute the angle by the last key of dpadKeys and the first key of dpadKeys
    DpadKeyItem lastItem = dpadKeys.back();
    int32_t currentKeyCode = lastItem.keyCode;
    DpadKeyTypeEnum currentDpadKeyType = lastItem.keyTypeEnum;
    int32_t angle = GetAngle(currentKeyCode, currentDpadKeyType, dpadKeys);
    if (angle == INVALID_VALUE) {
        HILOGW("cannot find valid angle value. the keycode is [%{private}d]'s", currentKeyCode);
        return;
    }
    MoveToTarget(context, keyEvent, mappingInfo, angle);
}

int32_t DpadKeyToTouchHandler::GetAngle(const int32_t currentKeyCode, const DpadKeyTypeEnum currentKeyType,
                                        std::vector<DpadKeyItem> &dpadKeys)
{
    // get angle by the currentKeyCode and the first keycode of dpadKeys
    std::string key;
    for (const auto &dpadKeyItem: dpadKeys) {
        if (dpadKeyItem.keyCode == currentKeyCode) {
            continue;
        }
        key = std::to_string(currentKeyType) + JOINER + std::to_string(dpadKeyItem.keyTypeEnum);
        if (keyAngleMap_.find(key) != keyAngleMap_.end()) {
            return keyAngleMap_[key];
        }
    }
    key = std::to_string(currentKeyType);
    if (keyAngleMap_.find(key) != keyAngleMap_.end()) {
        return keyAngleMap_[key];
    }
    return INVALID_VALUE;
}
}
}