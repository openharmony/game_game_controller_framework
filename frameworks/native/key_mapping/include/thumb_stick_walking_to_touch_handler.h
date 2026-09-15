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

#ifndef GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_WALKING_TO_TOUCH_HANDLER_H
#define GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_WALKING_TO_TOUCH_HANDLER_H

#include "key_to_touch_handler.h"

namespace OHOS {
namespace GameController {
class ThumbStickWalkingToTouchHandler : public BaseKeyToTouchHandler {
public:
    ThumbStickWalkingToTouchHandler() = default;
    ~ThumbStickWalkingToTouchHandler() override = default;

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
     * 清除激活状态并重置缓存的轴值。
     */
    void ResetState()
    {
        isActive_ = false;
        lastAxisX_ = 0.0;
        lastAxisY_ = 0.0;
    }

private:
    /**
     * @brief 处理摇杆轴事件，将摇杆偏移转换为触控事件
     *
     * 核心流程：
     * 1. 轴事件结束时（AXIS_END），若触控处于激活状态则去激活触控点；
     * 2. 更新缓存的轴值并判断是否处于死区内（摇杆回中或小幅偏移），
     *    激活状态下进入死区视为松开，去激活触控点；
     * 3. 超出死区时，根据轴值与映射半径计算目标触控坐标：
     *    - 未激活：申请指针ID，发送按下（DOWN）事件；
     *    - 已激活：发送移动（MOVE）事件。
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
     * 左摇杆对应 ABS_X/ABS_Y 轴，右摇杆对应 ABS_Z/ABS_RZ 轴。
     *
     * @param joystick 摇杆标识（左摇杆/右摇杆）
     * @param axisX 输出参数，摇杆X轴类型
     * @param axisY 输出参数，摇杆Y轴类型
     */
    void GetStickAxisTypes(int32_t joystick,
                           PointerEvent::AxisType &axisX,
                           PointerEvent::AxisType &axisY) const;

    /**
     * @brief 更新摇杆轴值的缓存
     *
     * 根据摇杆标识确定X/Y轴类型，若事件中携带对应轴数据，
     * 则更新成员变量中缓存的上一次轴值。
     *
     * @param pointerEvent 输入指针事件
     * @param mappingInfo 按键映射配置信息
     */
    void UpdateStickAxes(const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                         const KeyToTouchMappingInfo &mappingInfo);

    /**
     * @brief 去激活触控点，结束模拟触控
     *
     * 若存在历史触控点，则基于最后一次触控位置构建并发送抬手（UP）事件，
     * 随后释放指针ID，并重置激活状态、指针ID及缓存的轴值。
     *
     * @param context 输入转触控上下文
     * @param actionTime 事件触发时间戳
     */
    void DeactivatePointer(std::shared_ptr<InputToTouchContext> &context, int64_t actionTime);

    /**
     * @brief 摇杆死区阈值，轴值绝对值小于该值时视为摇杆回中
     */
    static constexpr double DEAD_ZONE = 0.15;

    /**
     * @brief 当前是否已激活模拟触控（已按下）
     */
    bool isActive_ = false;

    /**
     * @brief 当前模拟触控占用的指针ID
     */
    int32_t pointerId_ = 0;

    /**
     * @brief 缓存的上一次摇杆X轴值
     */
    double lastAxisX_ = 0.0;

    /**
     * @brief 缓存的上一次摇杆Y轴值
     */
    double lastAxisY_ = 0.0;
};
}
}

#endif // GAME_CONTROLLER_FRAMEWORK_THUMB_STICK_WALKING_TO_TOUCH_HANDLER_H
