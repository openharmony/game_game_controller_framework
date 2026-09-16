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

#include "stick_observation_task.h"
#include "gamecontroller_log.h"
#include <algorithm>
#include <cmath>

namespace OHOS {
namespace GameController {

StickObservationTask::StickObservationTask()
{
}

StickObservationTask::~StickObservationTask()
{
    // 析构时停止定时器，避免回调悬空
    StopTimer();
}

void StickObservationTask::StopTimer()
{
    ffrt_timer_t handleToStop;
    {
        // 加锁判断运行状态，取出定时器句柄并清空上下文
        std::lock_guard<ffrt::mutex> lock(taskLock_);
        if (!taskActive_) {
            return;
        }
        HILOGI("StickObservationTask: stop timer");
        taskActive_ = false;
        context_ = nullptr;
        handleToStop = timerHandle_;
    }

    // 锁外停止定时器，避免与定时器回调线程死锁
    ffrt_timer_stop(ffrt_qos_default, handleToStop);
}

void StickObservationTask::StartTimer(int32_t intervalMs)
{
    std::lock_guard<ffrt::mutex> lock(taskLock_);

    // 已处于运行状态时忽略重复启动
    if (taskActive_) {
        HILOGW("StickObservationTask: timer already running");
        return;
    }
    HILOGI("StickObservationTask: start timer interval=%{public}d", intervalMs);
    intervalMs_ = intervalMs;
    taskActive_ = true;
    firstActivation_ = true;

    // 周期性定时器，每个周期触发 OnTimerTick 回调
    timerHandle_ = ffrt_timer_start(ffrt_qos_default, static_cast<uint64_t>(intervalMs_),
                                    this, OnTimerTick, true);
}

void StickObservationTask::OnTimerTick(void* arg)
{
    // 定时器回调静态入口，转发到对应任务对象处理
    auto* task = static_cast<StickObservationTask*>(arg);
    task->RunTask();
}

void StickObservationTask::SetNeedCenterFirst(bool value)
{
    std::lock_guard<ffrt::mutex> lock(taskLock_);
    needCenterFirst_ = value;
}

bool StickObservationTask::IsActive()
{
    std::lock_guard<ffrt::mutex> lock(taskLock_);
    return taskActive_;
}

void StickObservationTask::UpdateJoystickData(double axisX, double axisY)
{
    // 加锁更新最新摇杆轴值，供定时任务读取
    std::lock_guard<ffrt::mutex> lock(taskLock_);
    rawStickX_ = axisX;
    rawStickY_ = axisY;
}

void StickObservationTask::BindContext(std::shared_ptr<InputToTouchContext> context,
                                       const KeyToTouchMappingInfo &mappingInfo,
                                       int32_t pointerId,
                                       bool isFpsMode)
{
    std::lock_guard<ffrt::mutex> lock(taskLock_);

    // 记录上下文、映射信息、指针ID与模式，步长取自映射配置
    context_ = context;
    mappingInfo_ = mappingInfo;
    pointerId_ = pointerId;
    isFpsMode_ = isFpsMode;
    stepX_ = mappingInfo.xStep;
    stepY_ = mappingInfo.yStep;

    // 初始化锚点、窗口边界、当前位置、滤波值与时间戳
    anchorX_ = mappingInfo.xValue;
    anchorY_ = mappingInfo.yValue;
    maxW_ = context->windowInfoEntity.maxWidth;
    maxH_ = context->windowInfoEntity.maxHeight;
    curX_ = anchorX_;
    curY_ = anchorY_;
    filteredX_ = 0.0;
    filteredY_ = 0.0;
    lastTick_ = std::chrono::steady_clock::now();
}

void StickObservationTask::RunTask()
{
    std::lock_guard<ffrt::mutex> lock(taskLock_);

    // 任务已停止或上下文已清空时，不处理
    if (!taskActive_) {
        return;
    }
    if (context_ == nullptr) {
        taskActive_ = false;
        return;
    }

    // 首个回调清除"先回中"标志
    if (needCenterFirst_) {
        needCenterFirst_ = false;
    }

    // 跳过启动后首个周期：DOWN事件刚发送，避免立即发送MOVE
    if (firstActivation_) {
        firstActivation_ = false;
        return;
    }

    double stickX = rawStickX_;
    double stickY = rawStickY_;
    std::shared_ptr<InputToTouchContext> ctx = context_;
    bool fps = isFpsMode_;
    double rawMag = std::sqrt(stickX * stickX + stickY * stickY);

    if (fps) {
        // FPS模式：死区内衰减滤波值并维持原位，死区外按速度模型移动
        if (rawMag < DEAD_ZONE) {
            filteredX_ *= DEAD_ZONE_DECAY_FACTOR;
            filteredY_ *= DEAD_ZONE_DECAY_FACTOR;
            lastTick_ = std::chrono::steady_clock::now();
            SendMoveEvent(ctx, curX_, curY_);
        } else {
            HandleActiveMovement(ctx, stickX, stickY);
        }
    } else {
        // 普通模式：目标位置 = 锚点 + 轴值 * 步长，越界时保持原位置
        int32_t targetX = anchorX_ + static_cast<int32_t>(stickX * static_cast<double>(stepX_));
        int32_t targetY = anchorY_ + static_cast<int32_t>(stickY * static_cast<double>(stepY_));
        if (targetX >= 0 && targetX <= maxW_) {
            curX_ = targetX;
        }
        if (targetY >= 0 && targetY <= maxH_) {
            curY_ = targetY;
        }
        SendMoveEvent(ctx, curX_, curY_);
    }
}

void StickObservationTask::SendMoveEvent(std::shared_ptr<InputToTouchContext> ctx, int32_t x, int32_t y)
{
    // 构建并在指定位置发送移动（MOVE）事件
    TouchEntity moveEntity;
    moveEntity.pointerId = pointerId_;
    moveEntity.pointerAction = PointerEvent::POINTER_ACTION_MOVE;
    moveEntity.xValue = x;
    moveEntity.yValue = y;
    moveEntity.actionTime = 0;
    BuildAndSendPointerEvent(ctx, moveEntity);
}

void StickObservationTask::HandleActiveMovement(std::shared_ptr<InputToTouchContext> ctx,
                                                double stickX, double stickY)
{
    double rawMag = std::sqrt(stickX * stickX + stickY * stickY);
    if (rawMag == 0.0) {
        return;
    }

    // 方向归一化，并将模长映射到死区外的有效区间
    double dirX = stickX / rawMag;
    double dirY = stickY / rawMag;
    double magnitude = (rawMag - DEAD_ZONE) / (1.0 - DEAD_ZONE);

    // 响应曲线：指数大于1，中心附近控制更细腻
    double curvedMag = std::pow(magnitude, RESPONSE_EXPONENT);

    // IIR低通滤波，平滑摇杆抖动
    filteredX_ = IIR_ALPHA * dirX * curvedMag + (1.0 - IIR_ALPHA) * filteredX_;
    filteredY_ = IIR_ALPHA * dirY * curvedMag + (1.0 - IIR_ALPHA) * filteredY_;

    // 结合时间增量计算本周期像素位移
    auto now = std::chrono::steady_clock::now();
    double deltaSec = std::chrono::duration<double>(now - lastTick_).count();
    lastTick_ = now;

    int32_t pixelDx = static_cast<int32_t>(filteredX_ * static_cast<double>(stepX_) * deltaSec);
    int32_t pixelDy = static_cast<int32_t>(filteredY_ * static_cast<double>(stepY_) * deltaSec);
    curX_ += pixelDx;
    curY_ += pixelDy;

    // 接近屏幕边缘：发送UP后在锚点附近重新DOWN（模拟手指回中再滑动），实现无限视角移动
    if (curX_ < EDGE_SAFE_MARGIN || curX_ > maxW_ - EDGE_SAFE_MARGIN ||
        curY_ < EDGE_SAFE_MARGIN || curY_ > maxH_ - EDGE_SAFE_MARGIN) {
        PointerEvent::PointerItem lastItem;
        if (ctx->pointerItems.find(pointerId_) != ctx->pointerItems.end()) {
            lastItem = ctx->pointerItems[pointerId_];
        }
        TouchEntity upEntity = BuildTouchUpEntity(lastItem, pointerId_,
                                                  PointerEvent::POINTER_ACTION_UP, 0);
        BuildAndSendPointerEvent(ctx, upEntity);
        curX_ = anchorX_ + pixelDx;
        curY_ = anchorY_ + pixelDy;
        TouchEntity downEntity = BuildTouchEntity(mappingInfo_, pointerId_,
                                                  PointerEvent::POINTER_ACTION_DOWN, 0);
        BuildAndSendPointerEvent(ctx, downEntity);
    }

    SendMoveEvent(ctx, curX_, curY_);
}

} // namespace GameController
} // namespace OHOS
