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
#ifndef GAME_CONTROLLER_FRAMEWORK_KEY_TO_TOUCH_HANDLER_H
#define GAME_CONTROLLER_FRAMEWORK_KEY_TO_TOUCH_HANDLER_H

#include <pointer_event.h>
#include <unordered_map>
#include <key_event.h>
#include <window.h>
#include <singleton.h>

#include "gamecontroller_keymapping_model.h"
#include "gamecontroller_log.h"
#include "ffrt.h"

namespace OHOS {
namespace GameController {
using namespace OHOS::MMI;

const int32_t MOUSE_LEFT_BUTTON_KEYCODE = 100000;
const int32_t MOUSE_RIGHT_BUTTON_KEYCODE = 100001;
const int32_t HALF_LENGTH = 2;
const int32_t MIN_EDGE = 1;

enum DpadKeyTypeEnum {
    DPAD_KEYTYPE_UNKNOWN = -1,
    DPAD_UP = 0,
    DPAD_DOWN = 1,
    DPAD_LEFT = 2,
    DPAD_RIGHT = 3,
};

struct DpadKeyItem {
    int64_t downTime = 0;
    int32_t keyCode = 0;
    DpadKeyTypeEnum keyTypeEnum = DPAD_KEYTYPE_UNKNOWN;

    DpadKeyItem() = default;

    DpadKeyItem(const KeyEvent::KeyItem &keyItem, const DpadKeyTypeEnum keyType)
    {
        keyCode = keyItem.GetKeyCode();
        downTime = keyItem.GetDownTime();
        keyTypeEnum = keyType;
    }
};

enum KeyCodeForPointer : int32_t {
    KEY_CODE_COMBINATION = 100000000,
    KEY_CODE_WALK = 100000001,
    KEY_CODE_SKILL = 100000002,
    KEY_CODE_OBSERVATION = 100000003,
    KEY_CODE_CROSSHAIR = 100000004,
    KEY_CODE_MOUSE_LEFT = 100000005,
    KEY_CODE_MOUSE_RIGHT = 100000006,
};

/**
 * the information of the current window
 */
struct WindowInfoEntity {
    int32_t windowId = 0;
    int32_t maxWidth = 0;
    int32_t maxHeight = 0;
    int32_t currentWidth = 0;
    int32_t currentHeight = 0;
    int32_t xPosition = 0;
    int32_t yPosition = 0;
    bool isFullScreen = false;
    int32_t xCenter = 0;
    int32_t yCenter = 0;

    void ParseRect(const OHOS::Rosen::Rect &rect)
    {
        xPosition = rect.posX_;
        yPosition = rect.posY_;
        currentWidth = static_cast<int32_t>(rect.width_);
        currentHeight = static_cast<int32_t>(rect.height_);
        xCenter = currentWidth / HALF_LENGTH;
        yCenter = currentHeight / HALF_LENGTH;
    };

    std::string ToString() const
    {
        std::string tmp = "windowId:" + std::to_string(windowId);
        tmp.append(", maxWidth:" + std::to_string(maxWidth));
        tmp.append(", maxHeight:" + std::to_string(maxHeight));
        tmp.append(", currentWidth:" + std::to_string(currentWidth));
        tmp.append(", currentHeight:" + std::to_string(currentHeight));
        tmp.append(", xPosition:" + std::to_string(xPosition));
        tmp.append(", yPosition:" + std::to_string(yPosition));
        tmp.append(", isFullScreen:" + std::to_string(isFullScreen));
        tmp.append(", xCenter:" + std::to_string(xCenter));
        tmp.append(", yCenter:" + std::to_string(yCenter));
        return tmp;
    }
};

/**
 * the context for handle KeyEvent or PointerEvent
 */
struct InputToTouchContext {
    DeviceTypeEnum deviceType = UNKNOWN;
    bool isMonitorMouse = false;
    std::unordered_map<int32_t, KeyToTouchMappingInfo> singleKeyMappings;
    std::unordered_map<int32_t, std::unordered_map<int32_t, KeyToTouchMappingInfo>> combinationKeyMappings;
    std::unordered_map<int32_t, KeyToTouchMappingInfo> mouseBtnKeyMappings;
    WindowInfoEntity windowInfoEntity;

    /**
     * key is keycode
     */
    std::unordered_map<int32_t, KeyToTouchMappingInfo> currentSingleKeyMap;

    bool isCombinationKeyOperating = false;
    KeyToTouchMappingInfo currentCombinationKey;

    bool isSkillOperating = false;
    KeyToTouchMappingInfo currentSkillKeyInfo;

    bool isPerspectiveObserving = false;
    KeyToTouchMappingInfo currentPerspectiveObserving;

    bool isCrosshairMode = false;
    bool isEnterCrosshairInfo = false;
    bool isMouseLeftFireOperating = false;
    KeyToTouchMappingInfo currentCrosshairInfo;

    bool isMouseRightClickOperating = false;

    bool isWalking = false;
    KeyToTouchMappingInfo currentWalking;

    /**
     * Used to store the last pointer information
     * key is pointerId, value is last pointer
     */
    std::unordered_map<int32_t, PointerEvent::PointerItem> pointerItems;
    PointerEvent::PointerItem lastMousePointer;

    /**
     * PointerId caching applied by keyCode
     * key is keycode, value is pointerId of keycode
     */
    std::unordered_map<int32_t, int32_t> pointerIdWithKeyCodeMap;

    InputToTouchContext() = default;

    InputToTouchContext(const DeviceTypeEnum &type, const WindowInfoEntity &windowInfo,
                        const std::vector<KeyToTouchMappingInfo> &mappingInfos);

    void SetCurrentSingleKeyInfo(const KeyToTouchMappingInfo &mappingInfo, const int32_t pointerId);

    bool HasSingleKeyDown(int32_t keyCode);

    void ResetCurrentSingleKeyInfo(int32_t keyCode);

    void SetCurrentCombinationKey(const KeyToTouchMappingInfo &mappingInfo, const int32_t pointerId);

    void ResetCurrentCombinationKey();

    void SetCurrentSkillKeyInfo(const KeyToTouchMappingInfo &mappingInfo, const int32_t pointerId);

    void ResetCurrentSkillKeyInfo();

    void SetCurrentObserving(const KeyToTouchMappingInfo &mappingInfo, const int32_t pointerId);

    void ResetCurrentObserving();

    void SetCurrentCrosshairInfo(const KeyToTouchMappingInfo &mappingInfo, const int32_t pointerId);

    void ResetCurrentCrosshairInfo();

    void SetCurrentWalking(const KeyToTouchMappingInfo &mappingInfo, const int32_t pointerId);

    void ResetCurrentWalking();

    bool IsMouseRightWalking();

    void SetCurrentMouseLeftClick(const int32_t pointerId);

    void ResetCurrentMouseLeftClick();

    void SetCurrentMouseRightClick(const int32_t pointerId);

    void ResetCurrentMouseRightClick();

    /**
     * Reset Temporary Variables when disable key-mapping
     */
    void ResetTempVariables();

    void ReleasePointerId(const int32_t keyCode);

    /**
     * Gey the applied pointerId by keycode
     * @param keyCode keyCode
     * @return first means whether pointerId exists
     */
    std::pair<bool, int32_t> GetPointerIdByKeyCode(const int32_t keyCode);
};

struct TouchEntity {
    int32_t pointerId = 0;
    int32_t pointerAction = 0;
    int32_t xValue;
    int32_t yValue;
    int64_t actionTime;
};

struct Point {
    double x;
    double y;
};

struct MouseMoveReq {
    /**
     * x position or y position of the current mouse  move point
     */
    int32_t currentMousePosition;

    /**
     * x position or y position of the last mouse move point
     */
    int32_t lastMousePosition;

    /**
     * x position or y position of the last touch
     */
    int32_t lastMovePosition;

    /**
     * maxWidth of window or maxHeight of window
     */
    int32_t maxEdge;

    /**
     * The step length of the mouse movement each time
     */
    int32_t step;
};

class PointerManager : public DelayedSingleton<PointerManager> {
DECLARE_DELAYED_SINGLETON(PointerManager)

public:
    /**
     * Apply pointerId
     * @return pointerId
     */
    int32_t ApplyPointerId();

    /**
     * Release the pointerId
     * @param pointerId pointerId
     */
    void ReleasePointerId(const int32_t pointerId);

private:
    ffrt::mutex locker;

    /**
     * The set of pointerId already are applied
     */
    std::unordered_set<int32_t> pointerIdCacheSet_;
};

class BaseKeyToTouchHandler {
public:
    virtual ~BaseKeyToTouchHandler()
    {
    };
public:
    static bool IsMouseLeftButtonEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    static bool IsMouseRightButtonEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    static bool IsMouseMoveEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    static bool IsKeyUpEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent);

public:
    virtual void HandleKeyEvent(std::shared_ptr<InputToTouchContext> &context,
                                const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                const DeviceInfo &deviceInfo,
                                const KeyToTouchMappingInfo &mappingInfo)
    {
        if (keyEvent == nullptr) {
            HILOGW("keyEvent is nullptr");
            return;
        }
        if (keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_DOWN) {
            HandleKeyDown(context, keyEvent, mappingInfo, deviceInfo);
        } else if (BaseKeyToTouchHandler::IsKeyUpEvent(keyEvent)) {
            HandleKeyUp(context, keyEvent, deviceInfo);
        } else {
            HILOGW("unknown [%{private}d]'s keyAction[%{public}d]", keyEvent->GetKeyCode(), keyEvent->GetKeyAction());
        }
    };

    virtual void HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                    const KeyToTouchMappingInfo &mappingInfo)
    {
    };

    virtual void ExitCrosshairKeyStatus()
    {
    };

protected:
    virtual void BuildAndSendPointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                          const TouchEntity &touchEntity);

    virtual void HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                               const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                               const KeyToTouchMappingInfo &mappingInfo,
                               const DeviceInfo &deviceInfo)
    {
    }

    virtual void HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                             const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                             const DeviceInfo &deviceInfo)
    {
    }

    TouchEntity BuildTouchEntity(const KeyToTouchMappingInfo &mappingInfo,
                                 const int32_t touchPointId,
                                 const int32_t pointerAction,
                                 const int64_t actionTime);

    TouchEntity BuildTouchUpEntity(const PointerEvent::PointerItem &lastPointItem,
                                   const int32_t touchPointId,
                                   const int32_t pointerAction,
                                   const int64_t actionTime);

    TouchEntity BuildMoveTouchEntity(const int32_t touchPointId,
                                     const Point &destPoint,
                                     const int64_t actionTime);

    /**
     * Calculate the distance between two points
     * @param centerPoint Center point
     * @param targetPoint Target Point
     * @return distance
     */
    double CalculateDistance(const Point &centerPoint, const Point &targetPoint);

    /**
     * Calculate the angle between two points
     * @param centerPoint Center point
     * @param targetPoint Target Point
     * @return angle
     */
    double CalculateAngle(const Point &centerPoint, const Point &targetPoint);

    /**
     * Obtain the target point based on the center point, radius, and angle.
     * @param centerPoint Center point
     * @param radius radius
     * @param angle angle
     * @return target point
     */
    Point ComputeTargetPoint(const Point &centerPoint, const double radius, const double angle);

    void ComputeTouchPointByMouseMoveEvent(std::shared_ptr<InputToTouchContext> &context,
                                           const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                           const KeyToTouchMappingInfo &mappingInfo,
                                           const int32_t &touchPointId);

    /**
     * Get all pressed valid dpad keys from KeyEvent. and sort by downTime asc
     * @param keyEvent KeyEvent
     * @param deviceInfo DeviceInfo
     * @param mapping KeyToTouchMappingInfo
     * @return all pressed valid dpad keys
     */
    std::vector<DpadKeyItem> CollectValidDpadKeys(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                  const DeviceInfo &deviceInfo,
                                                  const KeyToTouchMappingInfo &mapping);

    DpadKeyTypeEnum GetDpadKeyType(const int32_t keyCode, const KeyToTouchMappingInfo &mappingInfo);

private:

    void SortDpadKeys(std::vector<DpadKeyItem> &dpadKeys);

    int32_t ComputeMovePositionForX(std::shared_ptr<InputToTouchContext> &context,
                                    const PointerEvent::PointerItem &currentPointItem,
                                    const PointerEvent::PointerItem &lastMovePoint,
                                    const KeyToTouchMappingInfo &mappingInfo);

    int32_t ComputeMovePositionForY(std::shared_ptr<InputToTouchContext> &context,
                                    const PointerEvent::PointerItem &currentPointItem,
                                    const PointerEvent::PointerItem &lastMovePoint,
                                    const KeyToTouchMappingInfo &mappingInfo);

    /**
     * compute x position or y position for move touch
     * @param mouseMoveReq MouseMoveReq
     * @return return x position or y position for move touch
     */
    int32_t ComputeMovePosition(MouseMoveReq mouseMoveReq);

    PointerEvent::PointerItem BuildPointerItem(std::shared_ptr<InputToTouchContext> &context,
                                               const TouchEntity &touchEntity);
};

}
}
#endif //GAME_CONTROLLER_FRAMEWORK_KEY_TO_TOUCH_HANDLER_H
