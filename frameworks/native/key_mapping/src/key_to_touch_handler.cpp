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

#include <cmath>
#include <window_input_intercept_client.h>
#include "key_to_touch_handler.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t MOUSE_LEFT_BUTTON_ID = 0;
const int32_t MOUSE_RIGHT_BUTTON_ID = 1;
const int32_t DEVICE_ID = 3;
const double ANGLE = 180.0;
const int32_t TOUCH_RANGE = 10;
const int32_t START_POINTER_ID = 3;
}

void BaseKeyToTouchHandler::BuildAndSendPointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                                     const TouchEntity &touchEntity)
{
    if (context == nullptr) {
        return;
    }
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    if (pointerEvent == nullptr) {
        HILOGE("Create PointerEvent failed.");
        return;
    }
    PointerEvent::PointerItem pointerItem = BuildPointerItem(context, touchEntity);
    if (touchEntity.pointerAction == PointerEvent::POINTER_ACTION_UP) {
        if (context->pointerItems.find(touchEntity.pointerId) != context->pointerItems.end()) {
            context->pointerItems.erase(touchEntity.pointerId);
        }
    } else {
        context->pointerItems[touchEntity.pointerId] = pointerItem;
    }
    pointerEvent->AddPointerItem(pointerItem);
    pointerEvent->SetDeviceId(DEVICE_ID);
    pointerEvent->SetActionTime(touchEntity.actionTime);
    pointerEvent->SetAgentWindowId(context->windowInfoEntity.windowId);
    pointerEvent->SetTargetWindowId(context->windowInfoEntity.windowId);

    pointerEvent->SetId(std::numeric_limits<int32_t>::max());
    pointerEvent->SetPointerAction(touchEntity.pointerAction);
    pointerEvent->SetPointerId(touchEntity.pointerId);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    HILOGD("pointer is [%{public}s].", pointerEvent->ToString().c_str());
    Rosen::WindowInputInterceptClient::SendInputEvent(pointerEvent);
}

PointerEvent::PointerItem BaseKeyToTouchHandler::BuildPointerItem(std::shared_ptr<InputToTouchContext> &context,
                                                                  const TouchEntity &touchEntity)
{
    PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(touchEntity.pointerId);
    pointerItem.SetOriginPointerId(touchEntity.pointerId);
    pointerItem.SetWindowX(touchEntity.xValue);
    pointerItem.SetWindowY(touchEntity.yValue);
    pointerItem.SetDeviceId(DEVICE_ID);
    pointerItem.SetDownTime(touchEntity.actionTime);
    pointerItem.SetPressure(1);
    pointerItem.SetWidth(TOUCH_RANGE);
    pointerItem.SetHeight(TOUCH_RANGE);
    pointerItem.SetTargetWindowId(context->windowInfoEntity.windowId);
    return pointerItem;
}

TouchEntity BaseKeyToTouchHandler::BuildTouchEntity(const KeyToTouchMappingInfo &mappingInfo,
                                                    const int32_t touchPointId,
                                                    const int32_t pointerAction,
                                                    const int64_t actionTime)
{
    TouchEntity touchEntity;
    touchEntity.pointerId = touchPointId;
    touchEntity.xValue = mappingInfo.xValue;
    touchEntity.yValue = mappingInfo.yValue;
    touchEntity.pointerAction = pointerAction;
    touchEntity.actionTime = actionTime;
    return touchEntity;
}

TouchEntity BaseKeyToTouchHandler::BuildTouchUpEntity(const PointerEvent::PointerItem &lastPointItem,
                                                      const int32_t touchPointId,
                                                      const int32_t pointerAction,
                                                      const int64_t actionTime)
{
    TouchEntity touchEntity;
    touchEntity.pointerId = touchPointId;
    touchEntity.xValue = lastPointItem.GetWindowX();
    touchEntity.yValue = lastPointItem.GetWindowY();
    touchEntity.pointerAction = pointerAction;
    touchEntity.actionTime = actionTime;
    return touchEntity;
}

TouchEntity BaseKeyToTouchHandler::BuildMoveTouchEntity(const int32_t touchPointId,
                                                        const Point &destPoint,
                                                        const int64_t actionTime)
{
    TouchEntity touchEntity;
    touchEntity.pointerId = touchPointId;
    touchEntity.xValue = static_cast<int32_t>(destPoint.x);
    touchEntity.yValue = static_cast<int32_t>(destPoint.y);
    touchEntity.pointerAction = PointerEvent::POINTER_ACTION_MOVE;
    touchEntity.actionTime = actionTime;
    return touchEntity;
}

double BaseKeyToTouchHandler::CalculateAngle(const Point &centerPoint, const Point &targetPoint)
{
    double dx = targetPoint.x - centerPoint.x;
    double dy = targetPoint.y - centerPoint.y;
    return std::atan2(dy, dx) * (ANGLE / M_PI);
}

double BaseKeyToTouchHandler::CalculateDistance(const Point &centerPoint, const Point &targetPoint)
{
    double dx = targetPoint.x - centerPoint.x;
    double dy = targetPoint.y - centerPoint.y;
    return sqrt(dx * dx + dy * dy);
}

Point BaseKeyToTouchHandler::ComputeTargetPoint(const Point &centerPoint, const double radius, const double angle)
{
    double thetaRadians = angle * (M_PI / ANGLE);
    Point point;
    point.x = centerPoint.x + radius * cos(thetaRadians);
    point.y = centerPoint.y + radius * sin(thetaRadians);
    return point;
}

void BaseKeyToTouchHandler::ComputeTouchPointByMouseMoveEvent(std::shared_ptr<InputToTouchContext> &context,
                                                              const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                              const KeyToTouchMappingInfo &mappingInfo,
                                                              const int32_t &touchPointId)
{
    if (pointerEvent->GetPointerAction() != PointerEvent::POINTER_ACTION_MOVE) {
        return;
    }
    if (context->pointerItems.find(touchPointId) == context->pointerItems.end()) {
        HILOGW("discard mouse move event, because cannot find the last move event");
        return;
    }

    PointerEvent::PointerItem lastMovePoint = context->pointerItems[touchPointId];

    // Get current mouse pointer
    PointerEvent::PointerItem currentPointItem;
    pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), currentPointItem);

    Point targetPoint;
    targetPoint.x = ComputeMovePositionForX(context, currentPointItem, lastMovePoint, mappingInfo);
    targetPoint.y = ComputeMovePositionForY(context, currentPointItem, lastMovePoint, mappingInfo);
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildMoveTouchEntity(touchPointId, targetPoint, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->lastMousePointer = currentPointItem;
}

int32_t BaseKeyToTouchHandler::ComputeMovePositionForX(std::shared_ptr<InputToTouchContext> &context,
                                                       const PointerEvent::PointerItem &currentPointItem,
                                                       const PointerEvent::PointerItem &lastMovePoint,
                                                       const KeyToTouchMappingInfo &mappingInfo)
{
    MouseMoveReq mouseMoveReqForX;
    mouseMoveReqForX.currentMousePosition = currentPointItem.GetWindowX();
    mouseMoveReqForX.lastMousePosition = context->lastMousePointer.GetWindowX();
    mouseMoveReqForX.lastMovePosition = lastMovePoint.GetWindowX();
    mouseMoveReqForX.maxEdge = context->windowInfoEntity.maxWidth;
    mouseMoveReqForX.step = mappingInfo.xStep;
    return ComputeMovePosition(mouseMoveReqForX);
}

int32_t BaseKeyToTouchHandler::ComputeMovePositionForY(std::shared_ptr<InputToTouchContext> &context,
                                                       const PointerEvent::PointerItem &currentPointItem,
                                                       const PointerEvent::PointerItem &lastMovePoint,
                                                       const KeyToTouchMappingInfo &mappingInfo)
{
    MouseMoveReq mouseMoveReqForY;
    mouseMoveReqForY.currentMousePosition = currentPointItem.GetWindowY();
    mouseMoveReqForY.lastMousePosition = context->lastMousePointer.GetWindowY();
    mouseMoveReqForY.lastMovePosition = lastMovePoint.GetWindowY();
    mouseMoveReqForY.maxEdge = context->windowInfoEntity.maxHeight;
    mouseMoveReqForY.step = mappingInfo.yStep;
    return ComputeMovePosition(mouseMoveReqForY);
}

int32_t BaseKeyToTouchHandler::ComputeMovePosition(MouseMoveReq mouseMoveReq)
{
    int32_t result;
    if (mouseMoveReq.currentMousePosition > mouseMoveReq.lastMousePosition) {
        result = std::min(mouseMoveReq.lastMovePosition + mouseMoveReq.step, mouseMoveReq.maxEdge);
    } else if (mouseMoveReq.currentMousePosition < mouseMoveReq.lastMousePosition) {
        result = std::max(mouseMoveReq.lastMovePosition - mouseMoveReq.step, MIN_EDGE);
    } else {
        if (mouseMoveReq.currentMousePosition > MIN_EDGE &&
            mouseMoveReq.currentMousePosition < (mouseMoveReq.maxEdge - MIN_EDGE)) {
            return mouseMoveReq.lastMovePosition;
        }

        if (mouseMoveReq.currentMousePosition <= MIN_EDGE) {
            // mouse has moved to left or top edge
            result = std::max(mouseMoveReq.lastMovePosition - mouseMoveReq.step, MIN_EDGE);
        } else {
            result = std::min(mouseMoveReq.lastMovePosition + mouseMoveReq.step, mouseMoveReq.maxEdge);
        }
    }
    return result;
}

bool BaseKeyToTouchHandler::IsMouseLeftButtonEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (pointerEvent->GetSourceType() != PointerEvent::SOURCE_TYPE_MOUSE) {
        return false;
    }
    if (pointerEvent->GetButtonId() == MOUSE_LEFT_BUTTON_ID &&
        (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_BUTTON_DOWN ||
            pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_BUTTON_UP)) {
        return true;
    }
    return false;
}

bool BaseKeyToTouchHandler::IsMouseRightButtonEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (pointerEvent->GetSourceType() != PointerEvent::SOURCE_TYPE_MOUSE) {
        return false;
    }
    if (pointerEvent->GetButtonId() == MOUSE_RIGHT_BUTTON_ID &&
        (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_BUTTON_DOWN ||
            pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_BUTTON_UP)) {
        return true;
    }
    return false;
}

bool BaseKeyToTouchHandler::IsMouseMoveEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (pointerEvent->GetSourceType() != PointerEvent::SOURCE_TYPE_MOUSE) {
        return false;
    }
    return pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_MOVE;
}

std::vector<DpadKeyItem> BaseKeyToTouchHandler::CollectValidDpadKeys(
    const std::shared_ptr<MMI::KeyEvent> &keyEvent,
    const DeviceInfo &deviceInfo,
    const KeyToTouchMappingInfo &mapping)
{
    std::vector<KeyEvent::KeyItem> keyItems = keyEvent->GetKeyItems();
    std::vector<DpadKeyItem> dpadKeys;
    int32_t deviceId = keyEvent->GetDeviceId();
    for (const auto &keyItem: keyItems) {
        if (!keyItem.IsPressed() || keyItem.GetDeviceId() != deviceId ||
            keyItem.GetDownTime() < deviceInfo.onlineTime) {
            continue;
        }
        DpadKeyTypeEnum type = GetDpadKeyType(keyItem.GetKeyCode(), mapping);
        if (type == DPAD_KEYTYPE_UNKNOWN) {
            continue;
        }
        dpadKeys.push_back(DpadKeyItem(keyItem, type));
    }
    SortDpadKeys(dpadKeys);
    return dpadKeys;
}

DpadKeyTypeEnum BaseKeyToTouchHandler::GetDpadKeyType(const int32_t keyCode, const KeyToTouchMappingInfo &mappingInfo)
{
    DpadKeyCodeEntity dpad = mappingInfo.dpadKeyCodeEntity;
    if (keyCode == dpad.up) {
        return DPAD_UP;
    } else if (keyCode == dpad.down) {
        return DPAD_DOWN;
    } else if (keyCode == dpad.left) {
        return DPAD_LEFT;
    } else if (keyCode == dpad.right) {
        return DPAD_RIGHT;
    } else {
        return DPAD_KEYTYPE_UNKNOWN;
    }
}

void BaseKeyToTouchHandler::SortDpadKeys(std::vector<DpadKeyItem> &dpadKeys)
{
    //Sort in ascending order by downTime.
    std::sort(dpadKeys.begin(), dpadKeys.end(),
              [](const DpadKeyItem &a, const DpadKeyItem &b) {
                  return a.downTime < b.downTime;
              });
}

bool BaseKeyToTouchHandler::IsKeyUpEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    /**
     * 如果先按住shift，再按住ctrl,然后释放ctrl，不会收到KEY_ACTION_UP，而是KEY_ACTION_CANCEL事件
     * 因此这个事件等同于KEY_ACTION_UP
     */
    return keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_UP ||
        keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_CANCEL;
}

InputToTouchContext::InputToTouchContext(const DeviceTypeEnum &type,
                                         const WindowInfoEntity &windowInfo,
                                         const std::vector<KeyToTouchMappingInfo> &mappingInfos)
{
    deviceType = type;
    windowInfoEntity = windowInfo;
    int32_t lastKeyCode;
    int32_t firstKeyCode;
    for (auto &mappingInfo: mappingInfos) {
        if (mappingInfo.mappingType == COMBINATION_KEY_TO_TOUCH) {
            if (mappingInfo.combinationKeys.size() != MAX_COMBINATION_KEYS) {
                HILOGW("discard the combinationKeys because the combinationKeys.size is less than max");
                continue;
            }
            lastKeyCode = mappingInfo.combinationKeys.at(COMBINATION_LAST_KEYCODE_IDX);
            firstKeyCode = mappingInfo.combinationKeys.at(0);
            std::unordered_map<int32_t, KeyToTouchMappingInfo> map = combinationKeyMappings[lastKeyCode];
            map[firstKeyCode] = mappingInfo;
            combinationKeyMappings[lastKeyCode] = map;
        } else if (mappingInfo.mappingType == DPAD_KEY_TO_TOUCH
            || mappingInfo.mappingType == KEY_BOARD_OBSERVATION_TO_TOUCH) {
            singleKeyMappings[mappingInfo.dpadKeyCodeEntity.up] = mappingInfo;
            singleKeyMappings[mappingInfo.dpadKeyCodeEntity.down] = mappingInfo;
            singleKeyMappings[mappingInfo.dpadKeyCodeEntity.left] = mappingInfo;
            singleKeyMappings[mappingInfo.dpadKeyCodeEntity.right] = mappingInfo;
        } else if (mappingInfo.mappingType == MOUSE_RIGHT_KEY_WALKING_TO_TOUCH
            || mappingInfo.mappingType == MOUSE_OBSERVATION_TO_TOUCH
            || mappingInfo.mappingType == MOUSE_RIGHT_KEY_CLICK_TO_TOUCH) {
            mouseBtnKeyMappings[MOUSE_RIGHT_BUTTON_KEYCODE] = mappingInfo;
            isMonitorMouse = true;
        } else if (mappingInfo.mappingType == MOUSE_LEFT_FIRE_TO_TOUCH) {
            mouseBtnKeyMappings[MOUSE_LEFT_BUTTON_KEYCODE] = mappingInfo;
            isMonitorMouse = true;
        } else if (mappingInfo.mappingType == SKILL_KEY_TO_TOUCH
            || mappingInfo.mappingType == CROSSHAIR_KEY_TO_TOUCH
            || mappingInfo.mappingType == OBSERVATION_KEY_TO_TOUCH) {
            singleKeyMappings[mappingInfo.keyCode] = mappingInfo;
            isMonitorMouse = true;
        } else if (mappingInfo.mappingType == SINGE_KEY_TO_TOUCH) {
            singleKeyMappings[mappingInfo.keyCode] = mappingInfo;
        } else {
            HILOGW("unknown mappingType[%{public}d]", static_cast<int32_t>(mappingInfo.mappingType));
        }
    }
}

bool InputToTouchContext::HasSingleKeyDown(const int32_t keyCode)
{
    return currentSingleKeyMap.count(keyCode) != 0;
}

void InputToTouchContext::SetCurrentSingleKeyInfo(const KeyToTouchMappingInfo &mappingInfo, const int32_t pointerId)
{
    currentSingleKeyMap[mappingInfo.keyCode] = mappingInfo;
    pointerIdWithKeyCodeMap[mappingInfo.keyCode] = pointerId;
}

void InputToTouchContext::ResetCurrentSingleKeyInfo(const int32_t keyCode)
{
    currentSingleKeyMap.erase(keyCode);
    ReleasePointerId(keyCode);
}

void InputToTouchContext::ResetCurrentCombinationKey()
{
    currentCombinationKey = KeyToTouchMappingInfo();
    isCombinationKeyOperating = false;
    ReleasePointerId(KEY_CODE_COMBINATION);
}

void InputToTouchContext::ResetCurrentSkillKeyInfo()
{
    currentSkillKeyInfo = KeyToTouchMappingInfo();
    isSkillOperating = false;
    ReleasePointerId(KEY_CODE_SKILL);
}

void InputToTouchContext::ResetCurrentObserving()
{
    currentPerspectiveObserving = KeyToTouchMappingInfo();
    isPerspectiveObserving = false;
    lastMousePointer = PointerEvent::PointerItem();
    ReleasePointerId(KEY_CODE_OBSERVATION);
}

void InputToTouchContext::ResetCurrentCrosshairInfo()
{
    currentCrosshairInfo = KeyToTouchMappingInfo();
    isEnterCrosshairInfo = false;
    isCrosshairMode = false;
    lastMousePointer = PointerEvent::PointerItem();
    ReleasePointerId(KEY_CODE_CROSSHAIR);
}

void InputToTouchContext::ResetCurrentWalking()
{
    currentWalking = KeyToTouchMappingInfo();
    isWalking = false;
    ReleasePointerId(KEY_CODE_WALK);
}

bool InputToTouchContext::IsMouseRightWalking()
{
    return isWalking && currentWalking.mappingType == MOUSE_RIGHT_KEY_WALKING_TO_TOUCH;
}

void InputToTouchContext::ResetCurrentMouseRightClick()
{
    isMouseRightClickOperating = false;
    ReleasePointerId(KEY_CODE_MOUSE_RIGHT);
}

void InputToTouchContext::ResetTempVariables()
{
    currentSingleKeyMap.clear();
    for (const auto &pointerIdWithKeyCode: pointerIdWithKeyCodeMap) {
        DelayedSingleton<PointerManager>::GetInstance()->ReleasePointerId(pointerIdWithKeyCode.second);
    }
    pointerIdWithKeyCodeMap.clear();
    pointerItems.clear();
    ResetCurrentCombinationKey();
    ResetCurrentSkillKeyInfo();
    ResetCurrentObserving();
    ResetCurrentCrosshairInfo();
    ResetCurrentWalking();
    ResetCurrentMouseRightClick();
    ResetCurrentMouseLeftClick();
}

std::pair<bool, int32_t> InputToTouchContext::GetPointerIdByKeyCode(const int32_t keyCode)
{
    std::pair<bool, int32_t> pair;
    if (pointerIdWithKeyCodeMap.count(keyCode) == 0) {
        pair.first = false;
    } else {
        pair.first = true;
        pair.second = pointerIdWithKeyCodeMap[keyCode];
    }
    return pair;
}

void InputToTouchContext::SetCurrentSkillKeyInfo(const KeyToTouchMappingInfo &mappingInfo, const int32_t pointerId)
{
    isSkillOperating = true;
    currentSkillKeyInfo = mappingInfo;
    pointerIdWithKeyCodeMap[KEY_CODE_SKILL] = pointerId;
}

void InputToTouchContext::SetCurrentCombinationKey(const KeyToTouchMappingInfo &mappingInfo, const int32_t pointerId)
{
    isCombinationKeyOperating = true;
    currentCombinationKey = mappingInfo;
    pointerIdWithKeyCodeMap[KEY_CODE_COMBINATION] = pointerId;
}

void InputToTouchContext::SetCurrentCrosshairInfo(const KeyToTouchMappingInfo &mappingInfo, const int32_t pointerId)
{
    isCrosshairMode = true;
    isEnterCrosshairInfo = false;
    currentCrosshairInfo = mappingInfo;
    pointerIdWithKeyCodeMap[KEY_CODE_CROSSHAIR] = pointerId;
}

void InputToTouchContext::SetCurrentWalking(const KeyToTouchMappingInfo &mappingInfo, const int32_t pointerId)
{
    isWalking = true;
    currentWalking = mappingInfo;
    pointerIdWithKeyCodeMap[KEY_CODE_WALK] = pointerId;
}

void InputToTouchContext::SetCurrentObserving(const KeyToTouchMappingInfo &mappingInfo, const int32_t pointerId)
{
    isPerspectiveObserving = true;
    currentPerspectiveObserving = mappingInfo;
    pointerIdWithKeyCodeMap[KEY_CODE_OBSERVATION] = pointerId;
}

void InputToTouchContext::SetCurrentMouseLeftClick(const int32_t pointerId)
{
    isMouseLeftFireOperating = true;
    pointerIdWithKeyCodeMap[KEY_CODE_MOUSE_LEFT] = pointerId;
}

void InputToTouchContext::ResetCurrentMouseLeftClick()
{
    isMouseLeftFireOperating = false;
    ReleasePointerId(KEY_CODE_MOUSE_LEFT);
}

void InputToTouchContext::SetCurrentMouseRightClick(const int32_t pointerId)
{
    isMouseRightClickOperating = true;
    pointerIdWithKeyCodeMap[KEY_CODE_MOUSE_RIGHT] = pointerId;
}

void InputToTouchContext::ReleasePointerId(const int32_t keyCode)
{
    if (pointerIdWithKeyCodeMap.count(keyCode) != 0) {
        int32_t pointerId = pointerIdWithKeyCodeMap[keyCode];
        DelayedSingleton<PointerManager>::GetInstance()->ReleasePointerId(pointerId);
        pointerIdWithKeyCodeMap.erase(keyCode);
        pointerItems.erase(pointerId);
    }
}

PointerManager::PointerManager()
{
}

PointerManager::~PointerManager()
{
}

int32_t PointerManager::ApplyPointerId()
{
    std::lock_guard<ffrt::mutex> lock(locker);
    int32_t pointerId = START_POINTER_ID;
    while (true) {
        // Starting from 3, query an unused pointerId
        if (pointerIdCacheSet_.count(pointerId) == 0) {
            pointerIdCacheSet_.insert(pointerId);
            HILOGI("ApplyPointerId [%{public}d]", pointerId);
            return pointerId;
        }
        pointerId++;
    }
}

void PointerManager::ReleasePointerId(const int32_t pointerId)
{
    std::lock_guard<ffrt::mutex> lock(locker);
    pointerIdCacheSet_.erase(pointerId);
    HILOGI("ReleasePointerId [%{public}d]", pointerId);
}
}
}