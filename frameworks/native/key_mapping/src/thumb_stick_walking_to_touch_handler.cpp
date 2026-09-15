/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "thumb_stick_walking_to_touch_handler.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
void ThumbStickWalkingToTouchHandler::HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                                         const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                         const KeyToTouchMappingInfo &mappingInfo)
{
    // 参数有效性检查
    if (pointerEvent == nullptr || context == nullptr) {
        HILOGW("pointerEvent or context is nullptr");
        return;
    }

    // 仅处理轴事件（AXIS_BEGIN/AXIS_UPDATE/AXIS_END），非轴事件直接返回
    int32_t action = pointerEvent->GetPointerAction();
    if (action != PointerEvent::POINTER_ACTION_AXIS_BEGIN
        && action != PointerEvent::POINTER_ACTION_AXIS_UPDATE
        && action != PointerEvent::POINTER_ACTION_AXIS_END) {
        return;
    }
    HandleAxisEvent(context, pointerEvent, mappingInfo);
}

void ThumbStickWalkingToTouchHandler::GetStickAxisTypes(
    int32_t joystick,
    PointerEvent::AxisType &axisX,
    PointerEvent::AxisType &axisY) const
{
    if (joystick == STICK_LEFT) {
        // 左摇杆：X轴为ABS_X，Y轴为ABS_Y
        axisX = PointerEvent::AxisType::AXIS_TYPE_ABS_X;
        axisY = PointerEvent::AxisType::AXIS_TYPE_ABS_Y;
    } else {
        // 右摇杆：X轴为ABS_Z，Y轴为ABS_RZ
        axisX = PointerEvent::AxisType::AXIS_TYPE_ABS_Z;
        axisY = PointerEvent::AxisType::AXIS_TYPE_ABS_RZ;
    }
}

void ThumbStickWalkingToTouchHandler::DeactivatePointer(
    std::shared_ptr<InputToTouchContext> &context, int64_t actionTime)
{
    // 存在历史触控点时，在其最后位置发送UP事件
    if (context->pointerItems.find(pointerId_) != context->pointerItems.end()) {
        PointerEvent::PointerItem lastItem = context->pointerItems[pointerId_];
        TouchEntity upEntity = BuildTouchUpEntity(lastItem, pointerId_,
                                                  PointerEvent::POINTER_ACTION_UP, actionTime);
        BuildAndSendPointerEvent(context, upEntity);
    }

    // 释放指针ID，并重置激活状态、指针ID及缓存的轴值
    DelayedSingleton<PointerManager>::GetInstance()->ReleasePointerId(pointerId_);
    isActive_ = false;
    pointerId_ = 0;
    lastAxisX_ = 0.0;
    lastAxisY_ = 0.0;
}

void ThumbStickWalkingToTouchHandler::UpdateStickAxes(
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    const KeyToTouchMappingInfo &mappingInfo)
{
    PointerEvent::AxisType axisX;
    PointerEvent::AxisType axisY;
    GetStickAxisTypes(mappingInfo.joystick, axisX, axisY);

    // 事件中携带X轴数据时，更新缓存的X轴值
    if (pointerEvent->HasAxis(axisX)) {
        lastAxisX_ = pointerEvent->GetAxisValue(axisX);
    }

    // 事件中携带Y轴数据时，更新缓存的Y轴值
    if (pointerEvent->HasAxis(axisY)) {
        lastAxisY_ = pointerEvent->GetAxisValue(axisY);
    }
}

void ThumbStickWalkingToTouchHandler::HandleAxisEvent(std::shared_ptr<InputToTouchContext> &context,
                                                      const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                                      const KeyToTouchMappingInfo &mappingInfo)
{
    int32_t action = pointerEvent->GetPointerAction();
    int64_t actionTime = pointerEvent->GetActionTime();

    // 轴事件结束：若触控处于激活状态，发送UP事件并重置状态
    if (action == PointerEvent::POINTER_ACTION_AXIS_END) {
        if (isActive_) {
            DeactivatePointer(context, actionTime);
        }
        return;
    }

    // 更新缓存的轴值
    UpdateStickAxes(pointerEvent, mappingInfo);

    // 判断摇杆是否处于死区内（X、Y轴值均在死区阈值范围内）
    bool inDeadZone = (lastAxisX_ > -DEAD_ZONE && lastAxisX_ < DEAD_ZONE
        && lastAxisY_ > -DEAD_ZONE && lastAxisY_ < DEAD_ZONE);

    // 激活状态下摇杆回到死区，视为停止行走，结束模拟触控
    if (isActive_ && inDeadZone) {
        DeactivatePointer(context, actionTime);
        return;
    }

    // 摇杆处于死区内且未激活，无需处理
    if (inDeadZone) {
        return;
    }

    // 摇杆超出死区，将轴值转换为触控坐标：映射中心点 + 轴值 * 映射半径
    int32_t targetX = mappingInfo.xValue + static_cast<int32_t>(lastAxisX_ * mappingInfo.radius);
    int32_t targetY = mappingInfo.yValue + static_cast<int32_t>(lastAxisY_ * mappingInfo.radius);
    if (isActive_) {
        // 持续偏移：发送移动（MOVE）事件，触控点跟随摇杆位置变化
        TouchEntity touchEntity;
        touchEntity.pointerId = pointerId_;
        touchEntity.pointerAction = PointerEvent::POINTER_ACTION_MOVE;
        touchEntity.xValue = targetX;
        touchEntity.yValue = targetY;
        touchEntity.actionTime = actionTime;
        BuildAndSendPointerEvent(context, touchEntity);
    } else {
        // 首次超出死区：申请指针ID，发送按下（DOWN）事件
        pointerId_ = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
        isActive_ = true;
        TouchEntity touchEntity = BuildTouchEntity(mappingInfo, pointerId_,
                                                   PointerEvent::POINTER_ACTION_DOWN, actionTime);
        touchEntity.xValue = targetX;
        touchEntity.yValue = targetY;
        BuildAndSendPointerEvent(context, touchEntity);
    }
}
}
}
