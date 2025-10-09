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
#include <optional>
#include "input_manager.h"
#include "window_input_intercept.h"
#include "window_input_intercept_client.h"
#include "gamecontroller_log.h"
#include "input_event_callback.h"
#include "input_manager.h"
#include "multi_modal_input_mgt_service.h"
#include "gamecontroller_client_model.h"
#include "key_mapping_handle.h"
#include "key_to_touch_manager.h"

namespace OHOS {
namespace GameController {
namespace {
/**
 * Mapping between key values and key names
 */
const std::unordered_map<int32_t, std::pair<DeviceTypeEnum, std::string>> BUTTON_CODE_NAME_TRANSFORMATION = {
    {GamePadButtonTypeEnum::LeftShoulder,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "LeftShoulder")},
    {GamePadButtonTypeEnum::RightShoulder,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "RightShoulder")},
    {GamePadButtonTypeEnum::LeftTrigger,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "LeftTrigger")},
    {GamePadButtonTypeEnum::RightTrigger,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "RightTrigger")},

    {GamePadButtonTypeEnum::LeftThumbstickButton,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "LeftThumbstick")},
    {GamePadButtonTypeEnum::RightThumbstickButton,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "RightThumbstick")},
    {GamePadButtonTypeEnum::ButtonHome,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "ButtonHome")},
    {GamePadButtonTypeEnum::ButtonMenu,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "ButtonMenu")},
    {GamePadButtonTypeEnum::ButtonOptions,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "ButtonOptions")},
    {GamePadButtonTypeEnum::ButtonA,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "ButtonA")},
    {GamePadButtonTypeEnum::ButtonB,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "ButtonB")},
    {GamePadButtonTypeEnum::ButtonC,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "ButtonC")},
    {GamePadButtonTypeEnum::ButtonX,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "ButtonX")},
    {GamePadButtonTypeEnum::ButtonY,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "ButtonY")},

    {GamePadButtonTypeEnum::Dpad_UpButton,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "Dpad_UpButton")},
    {GamePadButtonTypeEnum::Dpad_DownButton,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "Dpad_DownButton")},
    {GamePadButtonTypeEnum::Dpad_LeftButton,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "Dpad_LeftButton")},
    {GamePadButtonTypeEnum::Dpad_RightButton,
        std::make_pair(DeviceTypeEnum::GAME_PAD, "Dpad_RightButton")}
};

const int64_t NS_TO_MS = 1000000LL;

/**
 * 1000000000ns == 1s
 */
const int64_t ONE_SECOND_BY_NS = 1000000000LL;

const int32_t KEY_ACTION_DOWN = 0;
const int32_t KEY_ACTION_UP = 1;
const int64_t US_TO_NS = 1000LL;
}

WindowInputInterceptConsumer::WindowInputInterceptConsumer() noexcept
{
    eventCallbackQueue_ = std::make_unique<ffrt::queue>("inputEventCallbackQueue",
                                                        ffrt::queue_attr().qos(ffrt::qos_default));
    timespec t1 = {};
    clock_gettime(CLOCK_MONOTONIC, &t1);
    int64_t monoTime = int64_t(t1.tv_sec) * ONE_SECOND_BY_NS + t1.tv_nsec;
    timespec t2 = {};
    clock_gettime(CLOCK_REALTIME, &t2);
    int64_t realTime = int64_t(t2.tv_sec) * ONE_SECOND_BY_NS + t2.tv_nsec;
    deltaTime_ = realTime - monoTime;
    HILOGI("deltaTime_ is %{public}lld", deltaTime_);
}

void WindowInputInterceptConsumer::OnInputEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    HILOGD("OnKeyEvent is %s", keyEvent->ToString().c_str());
    if (IsNotifyOpenTemplateConfigPage(keyEvent)) {
        return;
    }
    if (DelayedSingleton<KeyToTouchManager>::GetInstance()->DispatchKeyEvent(keyEvent)) {
        return;
    }
    ConsumeKeyInputEvent(keyEvent);
    Rosen::WindowInputInterceptClient::SendInputEvent(keyEvent);
}

void WindowInputInterceptConsumer::OnInputEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    HILOGD("OnInputEvent is %s", pointerEvent->ToString().c_str());
    if (DelayedSingleton<KeyToTouchManager>::GetInstance()->DispatchPointerEvent(pointerEvent)) {
        return;
    }
    ConsumePointerInputEvent(pointerEvent);
    Rosen::WindowInputInterceptClient::SendInputEvent(pointerEvent);
}

void WindowInputInterceptConsumer::ConsumeKeyInputEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    GamePadButtonEvent buttonEvent;
    if (keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_DOWN) {
        buttonEvent.keyAction = KEY_ACTION_DOWN;
    } else if (keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_UP ||
        keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_CANCEL) {
        buttonEvent.keyAction = KEY_ACTION_UP;
    } else {
        HILOGW("OnKeyEvent Unknown KeyAction is %{public}d",
               keyEvent->GetKeyAction());
        return;
    }
    buttonEvent.keyCode = keyEvent->GetKeyCode();
    if (BUTTON_CODE_NAME_TRANSFORMATION.find(buttonEvent.keyCode) == BUTTON_CODE_NAME_TRANSFORMATION.end()) {
        return;
    }
    buttonEvent.keyCodeName = BUTTON_CODE_NAME_TRANSFORMATION.at(buttonEvent.keyCode).second;
    buttonEvent.id = keyEvent->GetDeviceId();
    DeviceInfo deviceInfo = DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetDeviceInfo(buttonEvent.id);
    if (deviceInfo.UniqIsEmpty()) {
        HILOGW("OnKeyEvent Unknown device Id is %{public}d",
               buttonEvent.id);
        return;
    }
    buttonEvent.uniq = deviceInfo.uniq;
    buttonEvent.actionTime = (deltaTime_ + keyEvent->GetActionTime() * US_TO_NS) / NS_TO_MS; // 将距离开机启动时长转为系统时间
    std::vector<int32_t> pressedKeys = keyEvent->GetPressedKeys();
    for (auto keyCode: pressedKeys) {
        if (!PressedKeyIsValid(keyEvent, keyCode, deviceInfo)) {
            continue;
        }

        KeyInfo keyInfo;
        keyInfo.keyCode = keyCode;
        keyInfo.keyCodeName = BUTTON_CODE_NAME_TRANSFORMATION.at(keyCode).second;
        buttonEvent.keys.push_back(keyInfo);
    }
    DoGamePadKeyEventCallback(buttonEvent);
}

bool WindowInputInterceptConsumer::PressedKeyIsValid(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                     int32_t pressedKeyCode,
                                                     const DeviceInfo &deviceInfo)
{
    if (BUTTON_CODE_NAME_TRANSFORMATION.find(pressedKeyCode) == BUTTON_CODE_NAME_TRANSFORMATION.end()) {
        return false;
    }

    std::optional<MMI::KeyEvent::KeyItem> keyItem = keyEvent->GetKeyItem(pressedKeyCode);
    if (!keyItem.has_value()) {
        HILOGW("OnKeyEvent discard pressed keyCode [%{private}d], no keyItem",
               pressedKeyCode);
        return false;
    }

    if (keyItem.value().GetDeviceId() != keyEvent->GetDeviceId()) {
        HILOGD("OnKeyEvent discard pressed keyCode [%{private}d], the deviceId is invalid",
               pressedKeyCode);
        return false;
    }

    if (keyItem.value().GetDownTime() < deviceInfo.onlineTime) {
        /*
         * In some scenarios, when the device goes offline, the multi-mode input module does not receive the button
         * up event. As a result, the multi-mode input module considers that the button is still pressed.
         */
        HILOGE("OnKeyEvent discard pressed keyCode [%{private}d], downTime must be more than device onlineTime",
               pressedKeyCode);
        return false;
    }

    return true;
}

bool WindowInputInterceptConsumer::IsAxisEvent(const int32_t action)
{
    if (action != OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_BEGIN &&
        action != OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_UPDATE &&
        action != OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_END) {
        return false;
    }
    return true;
}

void WindowInputInterceptConsumer::ConsumePointerInputEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (pointerEvent->GetSourceType() != MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN
        && IsAxisEvent(pointerEvent->GetPointerAction())) {
        ConsumeGamePadAxisInputEvent(pointerEvent);
    }
}

void WindowInputInterceptConsumer::ConsumeGamePadAxisInputEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    int32_t id = pointerEvent->GetDeviceId();
    DeviceInfo deviceInfo = DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetDeviceInfo(id);
    if (deviceInfo.UniqIsEmpty()) {
        HILOGW("OnAxisEvent Unknown device Id is %{public}d", id);
        return;
    }
    int64_t actionTime = (deltaTime_ + pointerEvent->GetActionTime() * US_TO_NS) / NS_TO_MS;
    CallLeftThumbstickAxisEvent(id, deviceInfo, actionTime, pointerEvent);
    CallRightThumbstickAxisEvent(id, deviceInfo, actionTime, pointerEvent);
    CallLeftTriggerAxisEvent(id, deviceInfo, actionTime, pointerEvent);
    CallRightTriggerAxisEvent(id, deviceInfo, actionTime, pointerEvent);
    CallDpadAxisEvent(id, deviceInfo, actionTime, pointerEvent);
}

void WindowInputInterceptConsumer::DoGamePadKeyEventCallback(const GamePadButtonEvent &buttonEvent)
{
    eventCallbackQueue_->submit([buttonEvent, this] {
        DelayedSingleton<InputEventCallback>::GetInstance()->OnGamePadButtonEventCallback(buttonEvent);
    });
}

void WindowInputInterceptConsumer::DoAxisEventCallback(const GamePadAxisEvent &axisEvent)
{
    eventCallbackQueue_->submit([axisEvent, this] {
        DelayedSingleton<InputEventCallback>::GetInstance()->OnGamePadAxisEventCallback(axisEvent);
    });
}

void WindowInputInterceptConsumer::CallLeftThumbstickAxisEvent(const int32_t id,
                                                               const DeviceInfo &deviceInfo,
                                                               const int64_t actionTime,
                                                               const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (!pointerEvent->HasAxis(PointerEvent::AxisType::AXIS_TYPE_ABS_X)
        && !pointerEvent->HasAxis(PointerEvent::AxisType::AXIS_TYPE_ABS_Y)) {
        return;
    }
    GamePadAxisEvent axisEvent = BuildGamePadAxisEvent(id, deviceInfo, actionTime,
                                                       GamePadAxisSourceTypeEnum::LeftThumbstick);

    axisEvent.xValue = pointerEvent->GetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_X);
    axisEvent.yValue = pointerEvent->GetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_Y);
    DoAxisEventCallback(axisEvent);
}

void WindowInputInterceptConsumer::CallRightThumbstickAxisEvent(const int32_t id,
                                                                const DeviceInfo &deviceInfo,
                                                                const int64_t actionTime,
                                                                const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (!pointerEvent->HasAxis(PointerEvent::AxisType::AXIS_TYPE_ABS_Z)
        && !pointerEvent->HasAxis(PointerEvent::AxisType::AXIS_TYPE_ABS_RZ)) {
        return;
    }
    GamePadAxisEvent axisEvent = BuildGamePadAxisEvent(id, deviceInfo, actionTime,
                                                       GamePadAxisSourceTypeEnum::RightThumbstick);
    axisEvent.zValue = pointerEvent->GetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_Z);
    axisEvent.rzValue = pointerEvent->GetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_RZ);
    DoAxisEventCallback(axisEvent);
}

void WindowInputInterceptConsumer::CallLeftTriggerAxisEvent(const int32_t id,
                                                            const DeviceInfo &deviceInfo,
                                                            const int64_t actionTime,
                                                            const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (!pointerEvent->HasAxis(PointerEvent::AxisType::AXIS_TYPE_ABS_BRAKE)) {
        return;
    }
    GamePadAxisEvent axisEvent = BuildGamePadAxisEvent(id, deviceInfo, actionTime,
                                                       GamePadAxisSourceTypeEnum::LeftTriggerAxis);
    axisEvent.brakeValue = pointerEvent->GetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_BRAKE);
    DoAxisEventCallback(axisEvent);
}

void WindowInputInterceptConsumer::CallRightTriggerAxisEvent(const int32_t id,
                                                             const DeviceInfo &deviceInfo,
                                                             const int64_t actionTime,
                                                             const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (!pointerEvent->HasAxis(PointerEvent::AxisType::AXIS_TYPE_ABS_GAS)) {
        return;
    }
    GamePadAxisEvent axisEvent = BuildGamePadAxisEvent(id, deviceInfo, actionTime,
                                                       GamePadAxisSourceTypeEnum::RightTriggerAxis);
    axisEvent.gasValue = pointerEvent->GetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_GAS);
    DoAxisEventCallback(axisEvent);
}

void WindowInputInterceptConsumer::CallDpadAxisEvent(const int32_t id,
                                                     const DeviceInfo &deviceInfo,
                                                     const int64_t actionTime,
                                                     const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (!pointerEvent->HasAxis(PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0X)
        && !pointerEvent->HasAxis(PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0Y)) {
        return;
    }
    GamePadAxisEvent axisEvent = BuildGamePadAxisEvent(id, deviceInfo, actionTime,
                                                       GamePadAxisSourceTypeEnum::Dpad);
    axisEvent.hatxValue = pointerEvent->GetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0X);
    axisEvent.hatyValue = pointerEvent->GetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0Y);
    DoAxisEventCallback(axisEvent);
}

GamePadAxisEvent WindowInputInterceptConsumer::BuildGamePadAxisEvent(const int32_t id, const DeviceInfo &deviceInfo,
                                                                     const int64_t actionTime,
                                                                     const GamePadAxisSourceTypeEnum sourceTypeEnum)
{
    GamePadAxisEvent axisEvent;
    axisEvent.id = id;
    axisEvent.uniq = deviceInfo.uniq;
    axisEvent.axisSourceType = sourceTypeEnum;
    axisEvent.actionTime = actionTime;
    return axisEvent;
}

bool WindowInputInterceptConsumer::IsNotifyOpenTemplateConfigPage(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    return DelayedSingleton<KeyMappingHandle>::GetInstance()->IsNotifyOpenTemplateConfigPage(keyEvent);
}

WindowInputIntercept::~WindowInputIntercept()
{
}

WindowInputIntercept::WindowInputIntercept()
{
}

void WindowInputIntercept::RegisterWindowInputIntercept(const int32_t deviceId)
{
    std::lock_guard<std::mutex> lock(registerMutex_);
    if (consumer_ == nullptr) {
        consumer_ = std::make_shared<WindowInputInterceptConsumer>();
    }
    if (registerDeviceIdSet_.find(deviceId) != registerDeviceIdSet_.end()) {
        return;
    }
    registerDeviceIdSet_.insert(deviceId);
    Rosen::WindowInputInterceptClient::RegisterInputEventIntercept(deviceId, consumer_);
    HILOGI("Call RegisterWindowInputIntercept. the deviceId is %{public}d", deviceId);
}

void WindowInputIntercept::UnRegisterWindowInputIntercept(const int32_t deviceId)
{
    std::lock_guard<std::mutex> lock(registerMutex_);
    if (registerDeviceIdSet_.find(deviceId) != registerDeviceIdSet_.end()) {
        HILOGI("Call UnRegisterInputEventIntercept. the deviceId is %{public}d", deviceId);
        Rosen::WindowInputInterceptClient::UnRegisterInputEventIntercept(deviceId);
        registerDeviceIdSet_.erase(deviceId);
    }
}

void WindowInputIntercept::UnRegisterAllWindowInputIntercept()
{
    std::lock_guard<std::mutex> lock(registerMutex_);
    HILOGI("Call UnRegisterAllWindowInputIntercept.");
    for (auto deviceId: registerDeviceIdSet_) {
        HILOGI("Call UnRegisterInputEventIntercept. the deviceId is %{public}d", deviceId);
        Rosen::WindowInputInterceptClient::UnRegisterInputEventIntercept(deviceId);
    }
    registerDeviceIdSet_.clear();
}
}
}
