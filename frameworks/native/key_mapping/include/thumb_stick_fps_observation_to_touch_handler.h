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

#ifndef GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_FPS_OBSERVATION_TO_TOUCH_HANDLER_H
#define GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_FPS_OBSERVATION_TO_TOUCH_HANDLER_H

#include "key_to_touch_handler.h"
#include "stick_observation_task.h"

namespace OHOS {
namespace GameController {
class ThumbStickFpsObservationToTouchHandler : public BaseKeyToTouchHandler {
public:
    ThumbStickFpsObservationToTouchHandler() = default;

    /**
     * @brief 析构函数，停止观察定时器
     */
    ~ThumbStickFpsObservationToTouchHandler() override;

    /**
     * @brief 处理输入转触控的指针事件入口
     *
     * 过滤出摇杆的轴事件（开始/更新/结束），交给轴事件处理流程；
     * 其他类型的事件直接忽略。
     *
     * @param context 输入转触控上下文
     * @param pointerEvent 待处理的输入指针事件
     * @param mappingInfo 按键映射配置信息
     */
    void HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                            const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                            const KeyToTouchMappingInfo &mappingInfo) override;

    /**
     * @brief 重置处理器状态
     *
     * 停止观察定时器并清零缓存的轴值。
     */
    void ResetState();

    /**
     * @brief 停止观察定时器
     */
    void CancelTimer();

    /**
     * @brief 若FPS视角观察处于激活状态则释放触控点
     *
     * 激活时去激活观察（发送UP事件并重置状态）；未激活时仅重置状态。
     *
     * @param context 输入转触控上下文
     */
    void ReleaseIfActive(std::shared_ptr<InputToTouchContext> &context);

private:
    /**
     * @brief 处理摇杆轴事件，驱动FPS视角观察的启停
     *
     * 核心流程：
     * 1. 轴事件结束时（AXIS_END），若观察处于激活状态则去激活；
     * 2. 根据摇杆标识确定轴类型，读取并缓存轴值；
     * 3. 计算轴值模长并判断死区：摇杆回中时停止观察；
     * 4. 超出死区且未激活时，激活FPS视角观察。
     *
     * @param context 输入转触控上下文
     * @param pointerEvent 输入指针事件
     * @param mappingInfo 按键映射配置信息
     */
    void HandleAxisEvent(std::shared_ptr<InputToTouchContext> &context,
                         const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                         const KeyToTouchMappingInfo &mappingInfo);

    /**
     * @brief 获取摇杆对应的轴类型
     *
     * 左摇杆为ABS_X/ABS_Y轴，右摇杆为ABS_Z/ABS_RZ轴。
     *
     * @param joystick 摇杆标识（左摇杆/右摇杆）
     * @param axisZ 输出参数，X方向轴类型（左摇杆为ABS_X，右摇杆为ABS_Z）
     * @param axisRZ 输出参数，Y方向轴类型（左摇杆为ABS_Y，右摇杆为ABS_RZ）
     */
    void GetStickAxisTypes(int32_t joystick,
                           PointerEvent::AxisType &axisZ,
                           PointerEvent::AxisType &axisRZ) const;

    /**
     * @brief 更新摇杆轴值的缓存
     *
     * 事件中携带对应轴数据时，更新成员变量中缓存的轴值；
     * 仅携带单轴数据时保留另一轴的上次值。
     *
     * @param pointerEvent 输入指针事件
     * @param axisZ X方向轴类型（左摇杆为ABS_X，右摇杆为ABS_Z）
     * @param axisRZ Y方向轴类型（左摇杆为ABS_Y，右摇杆为ABS_RZ）
     */
    void UpdateStickAxes(const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                         PointerEvent::AxisType axisZ,
                         PointerEvent::AxisType axisRZ);

    /**
     * @brief 激活FPS视角观察
     *
     * 1. 以映射配置中的步长作为基准速度；
     * 2. 申请指针ID，在映射锚点位置发送按下（DOWN）事件；
     * 3. 按与任务相同的速度模型，以一个周期的时间增量预移动，
     *    立即发送首个移动（MOVE）事件；
     * 4. 以FPS模式绑定上下文，并按FPS周期启动定时器。
     *
     * @param context 输入转触控上下文
     * @param mappingInfo 按键映射配置信息
     * @param actionTime 事件触发时间戳
     * @param rawZ X方向轴值
     * @param rawRZ Y方向轴值
     * @param rawMag 轴值模长
     */
    void ActivateFpsObservation(std::shared_ptr<InputToTouchContext> &context,
                                const KeyToTouchMappingInfo &mappingInfo,
                                int64_t actionTime,
                                double rawZ, double rawRZ, double rawMag);

    /**
     * @brief 去激活FPS视角观察
     *
     * 停止观察定时器，若存在历史触控点则在其最后位置发送抬手（UP）事件，
     * 随后释放指针ID并重置状态。
     *
     * @param context 输入转触控上下文
     * @param actionTime 事件触发时间戳
     */
    void DeactivateFpsObservation(std::shared_ptr<InputToTouchContext> &context,
                                  int64_t actionTime);

    /**
     * @brief 视角观察任务，以FPS模式周期性发送移动（MOVE）事件
     */
    StickObservationTask task_;

    /**
     * @brief 当前模拟触控占用的指针ID
     */
    int32_t pointerId_ = 0;

    /**
     * @brief 缓存的上一次摇杆X轴值（左摇杆为ABS_X，右摇杆为ABS_Z）
     */
    double lastAxisZ_ = 0.0;

    /**
     * @brief 缓存的上一次摇杆Y轴值（左摇杆为ABS_Y，右摇杆为ABS_RZ）
     */
    double lastAxisRZ_ = 0.0;

    /**
     * @brief 摇杆死区阈值，轴值模长小于该值时视为摇杆回中
     */
    static constexpr double DEAD_ZONE = 0.05;

    /**
     * @brief 激活时预移动的初始时间增量（微秒），约一个FPS周期
     */
    static constexpr int64_t INIT_DELTA_US = 8000;

    /**
     * @brief 微秒到秒的换算系数
     */
    static constexpr double US_PER_SEC = 1000000.0;
};
}
}

#endif // GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_FPS_OBSERVATION_TO_TOUCH_HANDLER_H
