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

#include "thumb_stick_fps_observation_to_touch_handler.h"
#include "gamecontroller_log.h"
#include <algorithm>
#include <cmath>

namespace OHOS {
namespace GameController {

ThumbStickFpsObservationToTouchHandler::~ThumbStickFpsObservationToTouchHandler()
{
    // 析构时停止定时器，避免任务悬空回调
    CancelTimer();
}

void ThumbStickFpsObservationToTouchHandler::ResetState()
{
    // 停止定时器并清零缓存的轴值
    CancelTimer();
    lastAxisZ_ = 0.0;
    lastAxisRZ_ = 0.0;
}

void ThumbStickFpsObservationToTouchHandler::CancelTimer()
{
    task_.StopTimer();
}

void ThumbStickFpsObservationToTouchHandler::ReleaseIfActive(
    std::shared_ptr<InputToTouchContext> &context)
{
    // 激活状态：去激活观察（发送UP事件并重置状态）
    if (task_.IsActive()) {
        DeactivateFpsObservation(context, 0);
    } else {
        // 未激活：仅重置状态
        ResetState();
    }
}

void ThumbStickFpsObservationToTouchHandler::HandlePointerEvent(
    std::shared_ptr<InputToTouchContext> &context,
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    const KeyToTouchMappingInfo &mappingInfo)
{
    // 参数有效性检查
    if (pointerEvent == nullptr || context == nullptr) {
        return;
    }
    int32_t action = pointerEvent->GetPointerAction();

    // 仅处理轴事件（AXIS_BEGIN/AXIS_UPDATE/AXIS_END），非轴事件直接返回
    if (action != PointerEvent::POINTER_ACTION_AXIS_BEGIN
        && action != PointerEvent::POINTER_ACTION_AXIS_UPDATE
        && action != PointerEvent::POINTER_ACTION_AXIS_END) {
        return;
    }
    HandleAxisEvent(context, pointerEvent, mappingInfo);
}

void ThumbStickFpsObservationToTouchHandler::GetStickAxisTypes(
    int32_t joystick,
    PointerEvent::AxisType &axisZ,
    PointerEvent::AxisType &axisRZ) const
{
    if (joystick == STICK_LEFT) {
        // 左摇杆：X向轴为ABS_X，Y向轴为ABS_Y
        axisZ = PointerEvent::AxisType::AXIS_TYPE_ABS_X;
        axisRZ = PointerEvent::AxisType::AXIS_TYPE_ABS_Y;
    } else {
        // 右摇杆：X向轴为ABS_Z，Y向轴为ABS_RZ
        axisZ = PointerEvent::AxisType::AXIS_TYPE_ABS_Z;
        axisRZ = PointerEvent::AxisType::AXIS_TYPE_ABS_RZ;
    }
}

void ThumbStickFpsObservationToTouchHandler::UpdateStickAxes(
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    PointerEvent::AxisType axisZ,
    PointerEvent::AxisType axisRZ)
{
    // 事件中携带X向轴数据时，更新缓存的X向轴值（另一轴保留上次值）
    if (pointerEvent->HasAxis(axisZ)) {
        lastAxisZ_ = pointerEvent->GetAxisValue(axisZ);
    }
    // 事件中携带Y向轴数据时，更新缓存的Y向轴值（另一轴保留上次值）
    if (pointerEvent->HasAxis(axisRZ)) {
        lastAxisRZ_ = pointerEvent->GetAxisValue(axisRZ);
    }
}

void ThumbStickFpsObservationToTouchHandler::ActivateFpsObservation(
    std::shared_ptr<InputToTouchContext> &context,
    const KeyToTouchMappingInfo &mappingInfo,
    int64_t actionTime,
    double rawZ, double rawRZ, double rawMag)
{
    // 基准速度：直接使用映射配置中的步长
    int32_t baseSpeedX = mappingInfo.xStep;
    int32_t baseSpeedY = mappingInfo.yStep;

    // 申请指针ID，在映射锚点位置发送按下（DOWN）事件
    pointerId_ = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    TouchEntity downEntity = BuildTouchEntity(mappingInfo, pointerId_,
                                              PointerEvent::POINTER_ACTION_DOWN, actionTime);
    BuildAndSendPointerEvent(context, downEntity);

    // 计算初始位置：与任务相同的速度模型（方向*响应曲线*基准速度），
    // 按一个周期的时间增量预移动，保证与后续定时任务的首个周期衔接
    int32_t curX = mappingInfo.xValue;
    int32_t curY = mappingInfo.yValue;
    if (rawMag > 0.0) {
        double dirX = rawZ / rawMag;
        double dirY = rawRZ / rawMag;
        double magnitude = (rawMag - DEAD_ZONE) / (1.0 - DEAD_ZONE);
        double curvedMag = std::pow(magnitude, 1.5);
        double initDelta = static_cast<double>(INIT_DELTA_US) / US_PER_SEC;
        curX += static_cast<int32_t>(dirX * curvedMag * static_cast<double>(baseSpeedX) * initDelta);
        curY += static_cast<int32_t>(dirY * curvedMag * static_cast<double>(baseSpeedY) * initDelta);
    }

    // 立即发送首个移动（MOVE）事件，避免等待首个定时周期
    TouchEntity firstMove;
    firstMove.pointerId = pointerId_;
    firstMove.pointerAction = PointerEvent::POINTER_ACTION_MOVE;
    firstMove.xValue = curX;
    firstMove.yValue = curY;
    firstMove.actionTime = actionTime;
    BuildAndSendPointerEvent(context, firstMove);

    HILOGI("FPS started: anchor(%{public}d,%{public}d) speed(%{public}d,%{public}d)",
           mappingInfo.xValue, mappingInfo.yValue, baseSpeedX, baseSpeedY);

    // 以FPS模式绑定上下文，并按FPS周期启动定时器
    task_.BindContext(context, mappingInfo, pointerId_, true);
    task_.StartTimer(StickObservationTask::FPS_INTERVAL_MS);
}

void ThumbStickFpsObservationToTouchHandler::DeactivateFpsObservation(
    std::shared_ptr<InputToTouchContext> &context,
    int64_t actionTime)
{
    // 停止观察定时器
    task_.StopTimer();
    // 存在历史触控点时，在其最后位置发送UP事件
    if (context->pointerItems.find(pointerId_) != context->pointerItems.end()) {
        PointerEvent::PointerItem lastItem = context->pointerItems[pointerId_];
        TouchEntity upEntity = BuildTouchUpEntity(lastItem, pointerId_,
                                                  PointerEvent::POINTER_ACTION_UP, actionTime);
        BuildAndSendPointerEvent(context, upEntity);
    }
    // 释放指针ID并重置状态
    DelayedSingleton<PointerManager>::GetInstance()->ReleasePointerId(pointerId_);
    ResetState();
}

void ThumbStickFpsObservationToTouchHandler::HandleAxisEvent(
    std::shared_ptr<InputToTouchContext> &context,
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
    const KeyToTouchMappingInfo &mappingInfo)
{
    int32_t action = pointerEvent->GetPointerAction();
    int64_t actionTime = pointerEvent->GetActionTime();

    // 轴事件结束：若观察处于激活状态，发送UP事件并重置状态
    if (action == PointerEvent::POINTER_ACTION_AXIS_END) {
        if (task_.IsActive()) {
            DeactivateFpsObservation(context, actionTime);
        }
        return;
    }

    // 根据摇杆标识确定轴类型，事件中未携带任一轴数据时无需处理
    PointerEvent::AxisType axisZ;
    PointerEvent::AxisType axisRZ;
    GetStickAxisTypes(mappingInfo.joystick, axisZ, axisRZ);
    if (!pointerEvent->HasAxis(axisZ) && !pointerEvent->HasAxis(axisRZ)) {
        return;
    }

    // 读取轴值并更新缓存，随后计算轴值模长用于死区判断
    UpdateStickAxes(pointerEvent, axisZ, axisRZ);
    double rawMag = std::sqrt(lastAxisZ_ * lastAxisZ_ + lastAxisRZ_ * lastAxisRZ_);
    
    // 更新观察任务的摇杆数据，供定时任务计算移动量
    task_.UpdateJoystickData(lastAxisZ_, lastAxisRZ_);

    // 死区判断：摇杆回中时停止观察
    if (rawMag < DEAD_ZONE) {
        if (task_.IsActive()) {
            DeactivateFpsObservation(context, actionTime);
        }
        return;
    }
    // 超出死区且未激活时，激活FPS视角观察
    if (!task_.IsActive()) {
        ActivateFpsObservation(context, mappingInfo, actionTime, lastAxisZ_, lastAxisRZ_, rawMag);
    }
}

} // namespace GameController
} // namespace OHOS
