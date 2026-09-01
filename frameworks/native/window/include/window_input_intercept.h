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

#ifndef GAME_CONTROLLER_FRAMEWORK_WINDOW_INPUT_INTERCEPT_H
#define GAME_CONTROLLER_FRAMEWORK_WINDOW_INPUT_INTERCEPT_H

#include "gamecontroller_client_model.h"
#include "window_input_intercept_consumer.h"
#include <cpp/mutex.h>
#include <cpp/queue.h>
#include <singleton.h>
#include <unordered_set>

namespace OHOS {
namespace GameController {
class WindowInputInterceptConsumer : public Rosen::IInputEventInterceptConsumer {
public:
    WindowInputInterceptConsumer() noexcept;

    ~WindowInputInterceptConsumer() = default;

public:
    void OnInputEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent) override;

    void OnInputEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent) override;

    /**
     * Clears the cached processed device id when the device goes online or offline.
     * @param deviceId Device ID.
     */
    void ClearProcessedDeviceId(const int32_t deviceId);

private:
    /**
     * Consume key event
     * @param keyEvent Key Event
     */
    void ConsumeKeyInputEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent);

    /**
    * Consume pointer event
    * @param pointerEvent Pointer Event
    */
    void ConsumePointerInputEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    /**
     * Axis Event or Not
     * @param action Action of PointerEvent
     * @return true: axis event
     */
    bool IsAxisEvent(const int32_t action);

    /**
    * Consume GamePad's Axis Events
    * @param pointerEvent Pointer Event
    */
    void ConsumeGamePadAxisInputEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    void CallLeftThumbstickAxisEvent(const int32_t id,
                                     const DeviceInfo &deviceInfo,
                                     const int64_t actionTime,
                                     const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    void CallRightThumbstickAxisEvent(const int32_t id,
                                      const DeviceInfo &deviceInfo,
                                      const int64_t actionTime,
                                      const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    void CallLeftTriggerAxisEvent(const int32_t id,
                                  const DeviceInfo &deviceInfo,
                                  const int64_t actionTime,
                                  const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    void CallRightTriggerAxisEvent(const int32_t id,
                                   const DeviceInfo &deviceInfo,
                                   const int64_t actionTime,
                                   const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    void CallDpadAxisEvent(const int32_t id,
                           const DeviceInfo &deviceInfo,
                           const int64_t actionTime,
                           const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    /**
     * Executes the button event callback of the GamePad.
     * @param buttonEvent Button Event
     */
    void DoGamePadKeyEventCallback(const GamePadButtonEvent &buttonEvent);

    /**
     * Executes the unknown button event callback of the GamePad.
     * @param buttonEvent Button Event
     */
    void DoUnknownButtonEventCallback(const GamePadButtonEvent &buttonEvent);

    /**
     * Executes the axis event callback of the GamePad.
     * @param axisEvent Axis Event
     */
    void DoAxisEventCallback(const GamePadAxisEvent &axisEvent);

    GamePadAxisEvent BuildGamePadAxisEvent(const int32_t id, const DeviceInfo &deviceInfo, const int64_t actionTime,
                                           const GamePadAxisSourceTypeEnum sourceTypeEnum);

    /**
     * Check whether the pressed key is valid.
     * @param keyEvent Key Event
     * @param pressedKeyCode Pressed key code
     * @param deviceInfo Device Information
     * @return true: valid
     */
    bool PressedKeyIsValid(const std::shared_ptr<MMI::KeyEvent> &keyEvent, int32_t pressedKeyCode,
                           const DeviceInfo &deviceInfo);

    /**
     * Get key code name from either known or unknown button map.
     * @param keyCode Key code
     * @return Returns key code name if found; returns empty string otherwise.
     */
    std::string GetPressedKeyCodeName(int32_t keyCode);

    /**
     * Fill pressed keys info to button event.
     * @param keyEvent Key Event
     * @param deviceInfo Device Information
     * @param buttonEvent Button Event
     */
    void FillPressedKeys(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                         const DeviceInfo &deviceInfo,
                         GamePadButtonEvent &buttonEvent);

    /**
     * Indicates whether notify open template page.
     * @param keyEvent Key Event
     * @return true means notify open template page.
     */
    bool IsNotifyOpenTemplateConfigPage(const std::shared_ptr<MMI::KeyEvent> &keyEvent);

    /**
     * Checks whether the device has already processed known gamepad keys.
     * @param deviceId Device ID.
     * @return true means the device has processed known gamepad keys.
     */
    bool IsDeviceHasProcessedKeys(const int32_t deviceId);

    /**
     * Caches the device id that has already processed known gamepad keys.
     * @param deviceId Device ID.
     */
    void AddProcessedDeviceId(const int32_t deviceId);

private:
    /**
     * Event callback queue
     */
    std::unique_ptr<ffrt::queue> eventCallbackQueue_{nullptr};

    /**
     * Difference between the time span from startup to the current time and the actual timestamp, in ns.
     */
    int64_t deltaTime_ = 0;

    /**
     * Mutex for processedDeviceIdSet_
     */
    ffrt::mutex processedDeviceIdMutex_;

    /**
     * Set of device IDs that have already processed known gamepad keys.
     */
    std::unordered_set<int32_t> processedDeviceIdSet_;
};

class WindowInputIntercept : public DelayedSingleton<WindowInputIntercept> {
DECLARE_DELAYED_SINGLETON(WindowInputIntercept);

public:
    /**
     * Registers window input event interception for a specified deviceId.
     * @param deviceId Device ID.
     */
    void RegisterWindowInputIntercept(const int32_t deviceId);

    /**
     * Unregisters the window input event interception with a specified device ID.
     * @param deviceId Device ID.
     */
    void UnRegisterWindowInputIntercept(const int32_t deviceId);

    /**
     * Cancels window input event blocking for all devices.
     */
    void UnRegisterAllWindowInputIntercept();

    /**
     * Clears the cached processed device id when the device goes online or offline.
     * @param deviceId Device ID.
     */
    void ClearProcessedDeviceId(const int32_t deviceId);

private:
    std::mutex registerMutex_;

    /**
     * Set of device IDs that have been registered for listening.
     */
    std::unordered_set<int32_t> registerDeviceIdSet_;

    /**
     * Consumer of window input event
     */
    std::shared_ptr<WindowInputInterceptConsumer> consumer_;
};
}// namespace GameController
}// namespace OHOS
#endif//GAME_CONTROLLER_FRAMEWORK_WINDOW_INPUT_INTERCEPT_H
