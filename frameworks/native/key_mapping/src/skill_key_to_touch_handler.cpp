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

#include <window_input_intercept_client.h>
#include "skill_key_to_touch_handler.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
namespace {
const std::vector<std::pair<double, double>> SKILL_RANGE_SCALE_MAP = {
    {0,     1.508348148},
    {7.5,   1.454976296},
    {15,    1.405823704},
    {22.5,  1.356327407},
    {30,    1.301016296},
    {37.5,  1.285780741},
    {45,    1.257158519},
    {52.5,  1.229928889},
    {60,    1.151087407},
    {67.5,  1.100222222},
    {75,    1.054814815},
    {82.5,  1.0},
    {90,    1.0},
    {97.5,  1.0},
    {105,   1.054814815},
    {112.5, 1.100222222},
    {120,   1.151087407},
    {127.5, 1.229928889},
    {135,   1.257158519},
    {142.5, 1.285780741},
    {150,   1.301016296},
    {157.5, 1.356327407},
    {165,   1.405823704},
    {172.5, 1.454976296},
    {180,   1.508348148},
    {187.5, 1.454976296},
    {195,   1.405823704},
    {202.5, 1.356327407},
    {210,   1.301016296},
    {217.5, 1.257158519},
    {225,   1.207158519},
    {232.5, 1.157158519},
    {240,   1.101087407},
    {247.5, 1.050222222},
    {255,   1.004814815},
    {262.5, 0.92},
    {270,   0.85},
    {277.5, 0.92},
    {285,   1.004814815},
    {292.5, 1.050222222},
    {300,   1.101087407},
    {307.5, 1.157158519},
    {315,   1.207158519},
    {322.5, 1.257158519},
    {330,   1.301016296},
    {337.5, 1.356327407},
    {345,   1.405823704},
    {352.5, 1.454976296},
    {360,   1.508348148}
};

const double CIRCLE_ANGLE = 360.0;
const double DEFAULT_SKILL_RANGE_SCALE = 1.0;
const double Y_AXIS_OFFSET_RATE = 1.1;
}

void SkillKeyToTouchHandler::HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                                const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent == nullptr) {
        HILOGW("pointerEvent is nullptr");
        return;
    }
    if (IsNeedHandlePointEvent(context, pointerEvent)) {
        HandleMouseMove(context, pointerEvent);
    }
    Rosen::WindowInputInterceptClient::SendInputEvent(pointerEvent);
}

void SkillKeyToTouchHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                           const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                           const KeyToTouchMappingInfo &mappingInfo,
                                           const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (context->isSkillOperating) {
        if (context->currentSkillKeyInfo.keyCode != keyCode) {
            HILOGW("discard keyCode [%{private}d]. It's skill-operating now", keyCode);
        }
        return;
    }

    HILOGI("keyCode [%{private}d] convert to down event of skill-to-touch", keyCode);
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context->SetCurrentSkillKeyInfo(mappingInfo, pointerId);
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchEntity(context->currentSkillKeyInfo, pointerId,
                                               PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}

void SkillKeyToTouchHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                         const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                         const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    if (!context->isSkillOperating) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. No skill-operating",
               keyCode);
        return;
    }
    if (context->currentSkillKeyInfo.keyCode != keyCode) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. keyCode is not current skill-operating keycode",
               keyCode);
        return;
    }
    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_SKILL);
    if (!pair.first) {
        HILOGW("discard keyCode [%{private}d]'s keyup event. because cannot find the pointerId", keyCode);
        return;
    }
    int32_t pointerId = pair.second;
    if (context->pointerItems.find(pointerId) == context->pointerItems.end()) {
        HILOGW("discard button event, because cannot find the last point event");
        return;
    }
    PointerEvent::PointerItem lastMovePoint = context->pointerItems[pointerId];

    HILOGI("keyCode [%{private}d] convert to up event of skill-to-touch", keyCode);
    int64_t actionTime = keyEvent->GetActionTime();
    TouchEntity touchEntity = BuildTouchUpEntity(lastMovePoint, pointerId,
                                                 PointerEvent::POINTER_ACTION_UP, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
    context->ResetCurrentSkillKeyInfo();
}

void SkillKeyToTouchHandler::HandleMouseMove(std::shared_ptr<InputToTouchContext> &context,
                                             const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    // get the window's center point
    Point centerPoint;
    centerPoint.x = static_cast<double>(context->windowInfoEntity.xCenter);

    // Game character is not at the center of the y-axis
    centerPoint.y = static_cast<double>(context->windowInfoEntity.yCenter) * Y_AXIS_OFFSET_RATE;

    // get the mouse's point in window
    Point mousePoint;
    PointerEvent::PointerItem pointerItem;
    pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem);
    mousePoint.x = pointerItem.GetWindowX();
    mousePoint.y = pointerItem.GetWindowY();

    // compute the distance and angle with window's center point and mouse's point
    double distance = CalculateDistance(centerPoint, mousePoint);
    double angle = CalculateAngle(centerPoint, mousePoint);

    // compute the move point in skill key range
    double ellipseAngle = angle;
    if (ellipseAngle < 0) {
        ellipseAngle += CIRCLE_ANGLE;
    }

    // compute skill range scale
    double skillRangeScale = DEFAULT_SKILL_RANGE_SCALE;
    for (const auto &it: SKILL_RANGE_SCALE_MAP) {
        if (it.first <= ellipseAngle) {
            skillRangeScale = it.second;
        } else {
            break;
        }
    }
    double skillRangle = context->currentSkillKeyInfo.skillRange * skillRangeScale;
    double radius = (distance * context->currentSkillKeyInfo.radius) / skillRangle;
    
    Point skillCenterPoint;
    skillCenterPoint.x = context->currentSkillKeyInfo.xValue;
    skillCenterPoint.y = context->currentSkillKeyInfo.yValue;
    Point targetPoint = ComputeTargetPoint(skillCenterPoint, radius, angle);

    std::pair<bool, int32_t> pair = context->GetPointerIdByKeyCode(KEY_CODE_SKILL);
    if (!pair.first) {
        HILOGW("discard mouse move event. because cannot find the pointerId");
        return;
    }
    int32_t pointerId = pair.second;
    int64_t actionTime = pointerEvent->GetActionTime();
    TouchEntity touchEntity = BuildMoveTouchEntity(pointerId, targetPoint, actionTime);
    BuildAndSendPointerEvent(context, touchEntity);
}

bool SkillKeyToTouchHandler::IsNeedHandlePointEvent(std::shared_ptr<InputToTouchContext> &context,
                                                    const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (!BaseKeyToTouchHandler::IsMouseMoveEvent(pointerEvent)) {
        return false;
    }
    if (!context->isSkillOperating) {
        return false;
    }
    return true;
}
}
}