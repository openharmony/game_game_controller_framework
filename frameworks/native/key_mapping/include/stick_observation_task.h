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

#ifndef GAME_CONTROLLER_FRAMEWORK_STICK_OBSERVATION_TASK_H
#define GAME_CONTROLLER_FRAMEWORK_STICK_OBSERVATION_TASK_H

#include <chrono>
#include <memory>
#include "key_to_touch_handler.h"
#include "ffrt.h"

namespace OHOS {
namespace GameController {

/**
 * @brief 摇杆视角观察定时任务
 *
 * 继承 BaseKeyToTouchHandler 以复用触控事件构建能力。
 * 由处理器在激活视角观察时启动，按定时器周期读取最新摇杆数据，
 * 周期性发送移动（MOVE）事件模拟手指持续滑动：
 * - 普通模式：目标位置 = 锚点 + 轴值 * 步长，位置直接映射并限制在窗口内；
 * - FPS模式：基于响应曲线与IIR低通滤波的速度模型，
 *   按时间增量持续移动，接近屏幕边缘时通过UP/DOWN事件重新锚定。
 */
class StickObservationTask : public BaseKeyToTouchHandler {
public:
    StickObservationTask();

    /**
     * @brief 析构函数，停止定时器并释放资源
     */
    ~StickObservationTask();

    /**
     * @brief 启动观察定时器
     *
     * 已处于运行状态时仅打印告警并忽略；启动时重置首次激活标志，
     * 定时器周期性触发 OnTimerTick 回调。
     *
     * @param intervalMs 定时器周期，单位毫秒
     */
    void StartTimer(int32_t intervalMs);

    /**
     * @brief 停止观察定时器并清空上下文
     *
     * 未处于运行状态时直接返回；停止时清空持有的上下文，
     * 并在锁外调用 ffrt_timer_stop 避免死锁。
     */
    void StopTimer();

    /**
     * @brief 更新最新的摇杆轴值数据
     *
     * 由处理器在轴事件到来时调用，供定时任务计算移动量。
     *
     * @param axisX X方向轴值
     * @param axisY Y方向轴值
     */
    void UpdateJoystickData(double axisX, double axisY);

    /**
     * @brief 设置"先回中"标志
     *
     * @param value 是否需要先回中
     */
    void SetNeedCenterFirst(bool value);

    /**
     * @brief 查询观察任务是否处于运行状态
     *
     * @return true: 运行中 false: 已停止
     */
    bool IsActive();

    /**
     * @brief 绑定运行所需的上下文与映射配置
     *
     * 记录触控上下文、映射信息、指针ID与模式，步长取自映射配置，
     * 并初始化锚点、窗口边界、当前位置、滤波值与时间戳。
     *
     * @param context 输入转触控上下文
     * @param mappingInfo 按键映射配置信息
     * @param pointerId 模拟触控占用的指针ID
     * @param isFpsMode 是否为FPS模式
     */
    void BindContext(std::shared_ptr<InputToTouchContext> context,
                     const KeyToTouchMappingInfo &mappingInfo,
                     int32_t pointerId,
                     bool isFpsMode);

    /**
     * @brief 普通模式观察周期，单位毫秒
     */
    static constexpr int32_t OBSERVATION_INTERVAL_MS = 50;

    /**
     * @brief FPS模式观察周期，单位毫秒
     */
    static constexpr int32_t FPS_INTERVAL_MS = 20;

private:
    /**
     * @brief 定时器周期回调处理，执行一次观察移动逻辑
     *
     * 普通模式：目标位置 = 锚点 + 轴值 * 步长，越界时保持原位置；
     * FPS模式：死区内对滤波值衰减并维持原位，死区外按速度模型移动。
     */
    void RunTask();

    /**
     * @brief 定时器回调静态入口，转发到对应任务的 RunTask
     *
     * @param arg 任务对象指针
     */
    static void OnTimerTick(void* arg);

    /**
     * @brief 在指定位置发送移动（MOVE）触控事件
     *
     * @param ctx 输入转触控上下文
     * @param x 目标X坐标
     * @param y 目标Y坐标
     */
    void SendMoveEvent(std::shared_ptr<InputToTouchContext> ctx, int32_t x, int32_t y);

    /**
     * @brief FPS模式下的主动移动处理
     *
     * 对轴值做归一化、死区映射、响应曲线与IIR低通滤波，
     * 结合时间增量计算像素位移并更新当前位置；
     * 接近屏幕边缘时发送UP/DOWN事件重新锚定后继续移动。
     *
     * @param ctx 输入转触控上下文
     * @param stickX X方向轴值
     * @param stickY Y方向轴值
     */
    void HandleActiveMovement(std::shared_ptr<InputToTouchContext> ctx, double stickX, double stickY);

    /**
     * @brief 任务状态锁，保护任务全部共享状态
     */
    ffrt::mutex taskLock_;

    /**
     * @brief 定时任务是否处于运行状态
     */
    bool taskActive_ = false;

    /**
     * @brief 输入转触控上下文
     */
    std::shared_ptr<InputToTouchContext> context_;

    /**
     * @brief 按键映射配置信息
     */
    KeyToTouchMappingInfo mappingInfo_;

    /**
     * @brief 触控锚点X坐标（映射配置的按下位置）
     */
    int32_t anchorX_ = 0;

    /**
     * @brief 触控锚点Y坐标（映射配置的按下位置）
     */
    int32_t anchorY_ = 0;

    /**
     * @brief 窗口宽度上限（X坐标最大值）
     */
    int32_t maxW_ = 0;

    /**
     * @brief 窗口高度上限（Y坐标最大值）
     */
    int32_t maxH_ = 0;

    /**
     * @brief X方向移动步长（像素）
     */
    int32_t stepX_ = 0;

    /**
     * @brief Y方向移动步长（像素）
     */
    int32_t stepY_ = 0;

    /**
     * @brief 模拟触控占用的指针ID
     */
    int32_t pointerId_ = 0;

    /**
     * @brief 定时器周期，单位毫秒
     */
    int32_t intervalMs_ = 50;

    /**
     * @brief 最新的X方向摇杆轴值
     */
    double rawStickX_ = 0.0;

    /**
     * @brief 最新的Y方向摇杆轴值
     */
    double rawStickY_ = 0.0;

    /**
     * @brief 是否为FPS模式（速度模型移动）
     */
    bool isFpsMode_ = false;

    /**
     * @brief 定时器启动后是否尚未执行过首次回调
     */
    bool firstActivation_ = true;

    /**
     * @brief 当前触控点X坐标
     */
    int32_t curX_ = 0;

    /**
     * @brief 当前触控点Y坐标
     */
    int32_t curY_ = 0;

    /**
     * @brief IIR滤波后的X方向速度分量
     */
    double filteredX_ = 0.0;

    /**
     * @brief IIR滤波后的Y方向速度分量
     */
    double filteredY_ = 0.0;

    /**
     * @brief ffrt定时器句柄
     */
    ffrt_timer_t timerHandle_ = 0;

    /**
     * @brief 上次回调的时间戳，用于计算时间增量
     */
    std::chrono::steady_clock::time_point lastTick_;

    /**
     * @brief "先回中"标志，置位后首个回调会将其清除
     */
    bool needCenterFirst_ = false;

    /**
     * @brief 死区阈值，轴值模长小于该值视为摇杆回中
     */
    static constexpr double DEAD_ZONE = 0.05;

    /**
     * @brief 响应曲线指数，大于1时中心附近控制更细腻
     */
    static constexpr double RESPONSE_EXPONENT = 1.5;

    /**
     * @brief IIR低通滤波系数，越大响应越快、抖动越大
     */
    static constexpr double IIR_ALPHA = 0.4;

    /**
     * @brief FPS模式下死区内的滤波值衰减因子
     */
    static constexpr double DEAD_ZONE_DECAY_FACTOR = 0.7;

    /**
     * @brief 屏幕边缘安全边距（像素），触点接近边缘时触发重新锚定
     */
    static constexpr int32_t EDGE_SAFE_MARGIN = 50;
};

} // namespace GameController
} // namespace OHOS

#endif // GAME_CONTROLLER_FRAMEWORK_STICK_OBSERVATION_TASK_H
