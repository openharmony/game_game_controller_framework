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

class StickObservationTask : public BaseKeyToTouchHandler {
public:
    StickObservationTask();
    ~StickObservationTask();

    void StartTimer(int32_t intervalMs);
    void StopTimer();
    void UpdateJoystickData(double axisX, double axisY);
    void SetNeedCenterFirst(bool value);
    bool IsActive();
    bool IsFirstActivation();

    void BindContext(std::shared_ptr<InputToTouchContext> context,
                     const KeyToTouchMappingInfo &mappingInfo,
                     int32_t pointerId,
                     bool isFpsMode,
                     int32_t stepX,
                     int32_t stepY);

    static constexpr int32_t OBSERVATION_INTERVAL_MS = 50;
    static constexpr int32_t FPS_INTERVAL_MS = 20;

private:
    void RunTask();
    static void OnTimerTick(void* arg);

    void SendMoveEvent(std::shared_ptr<InputToTouchContext> ctx, int32_t x, int32_t y);
    void HandleActiveMovement(std::shared_ptr<InputToTouchContext> ctx, double stickX, double stickY);

    ffrt::mutex taskLock_;
    bool taskActive_ = false;

    std::shared_ptr<InputToTouchContext> context_;
    KeyToTouchMappingInfo mappingInfo_;
    int32_t anchorX_ = 0;
    int32_t anchorY_ = 0;
    int32_t maxW_ = 0;
    int32_t maxH_ = 0;
    int32_t stepX_ = 0;
    int32_t stepY_ = 0;
    int32_t pointerId_ = 0;
    int32_t intervalMs_ = 50;

    double rawStickX_ = 0.0;
    double rawStickY_ = 0.0;

    bool isFpsMode_ = false;
    bool firstActivation_ = true;
    int32_t curX_ = 0;
    int32_t curY_ = 0;
    double filteredX_ = 0.0;
    double filteredY_ = 0.0;

    ffrt_timer_t timerHandle_;
    std::chrono::steady_clock::time_point lastTick_;
    bool needCenterFirst_ = false;

    static constexpr double DEAD_ZONE = 0.05;
    static constexpr double RESPONSE_EXPONENT = 1.5;
    static constexpr double IIR_ALPHA = 0.4;
    static constexpr double DEAD_ZONE_DECAY_FACTOR = 0.7;
    static constexpr int32_t EDGE_SAFE_MARGIN = 50;
};

} // namespace GameController
} // namespace OHOS

#endif // GAME_CONTROLLER_FRAMEWORK_STICK_OBSERVATION_TASK_H
