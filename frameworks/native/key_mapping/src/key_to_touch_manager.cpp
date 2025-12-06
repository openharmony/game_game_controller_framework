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
#include <input_manager.h>
#include "key_to_touch_manager.h"
#include "multi_modal_input_mgt_service.h"
#include "skill_key_to_touch_handler.h"
#include "gamecontroller_log.h"
#include "mouse_observation_to_touch_handler.h"
#include "keyboard_observation_to_touch_handler.h"
#include "single_key_to_touch_handler.h"
#include "observation_key_to_touch_handler.h"
#include "crosshair_key_to_touch_handler.h"
#include "mouse_left_fire_to_touch_handler.h"
#include "combination_key_to_touch_handler.h"
#include "dpad_key_to_touch_handler.h"
#include "mouse_right_key_walking_to_touch_handler.h"
#include "mouse_right_key_click_to_touch_handler.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t DELAY_TIME_UNIT = 1000000;// 1s = 1000ms = 1000000μs
}

KeyToTouchManager::KeyToTouchManager()
{
    handleQueue_ = std::make_unique<ffrt::queue>("KeyToTouchHandleQueue",
                                                 ffrt::queue_attr().qos(ffrt::qos_default));
    mappingHandler_[MappingTypeEnum::SINGE_KEY_TO_TOUCH] = std::make_shared<SingleKeyToTouchHandler>();
    mappingHandler_[MappingTypeEnum::COMBINATION_KEY_TO_TOUCH] = std::make_shared<CombinationKeyToTouchHandler>();
    mappingHandler_[MappingTypeEnum::DPAD_KEY_TO_TOUCH] = std::make_shared<DpadKeyToTouchHandler>();
    mappingHandler_[MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH] =
        std::make_shared<MouseRightKeyWalkingToTouchHandler>();
    mappingHandler_[MappingTypeEnum::SKILL_KEY_TO_TOUCH] = std::make_shared<SkillKeyToTouchHandler>();
    mappingHandler_[MappingTypeEnum::OBSERVATION_KEY_TO_TOUCH] = std::make_shared<ObservationKeyToTouchHandler>();
    mappingHandler_[MappingTypeEnum::KEY_BOARD_OBSERVATION_TO_TOUCH] =
        std::make_shared<KeyboardObservationToTouchHandler>();
    mappingHandler_[MappingTypeEnum::MOUSE_OBSERVATION_TO_TOUCH] = std::make_shared<MouseObservationToTouchHandler>();
    mappingHandler_[MappingTypeEnum::CROSSHAIR_KEY_TO_TOUCH] = std::make_shared<CrosshairKeyToTouchHandler>();
    mappingHandler_[MappingTypeEnum::MOUSE_LEFT_FIRE_TO_TOUCH] = std::make_shared<MouseLeftFireToTouchHandler>();
    mappingHandler_[MappingTypeEnum::MOUSE_RIGHT_KEY_CLICK_TO_TOUCH]
        = std::make_shared<MouseRightKeyClickToTouchHandler>();
    handleQueue_->submit([this] {
        CheckPointerSendInterval();
    }, ffrt::task_attr().name("pointer-check-task").delay(DELAY_TIME_UNIT));
}

KeyToTouchManager::~KeyToTouchManager()
{
}

void KeyToTouchManager::SetSupportKeyMapping(bool isSupportKeyMapping,
                                             const std::unordered_set<int32_t> &deviceTypeSet)
{
    std::lock_guard<ffrt::mutex> lock(checkMutex_);
    isSupportKeyMapping_ = isSupportKeyMapping;
    supportDeviceTypeSet_ = deviceTypeSet;
}

bool KeyToTouchManager::DispatchKeyEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    if (keyEvent->GetKeyAction() != KeyEvent::KEY_ACTION_DOWN
        && keyEvent->GetKeyAction() != KeyEvent::KEY_ACTION_UP
        && keyEvent->GetKeyAction() != KeyEvent::KEY_ACTION_CANCEL) {
        return false;
    }
    std::lock_guard<ffrt::mutex> lock(checkMutex_);
    if (!IsCanEnableKeyMapping()) {
        return false;
    }
    int32_t keyCode = keyEvent->GetKeyCode();
    if (allMonitorKeys_.find(keyCode) == allMonitorKeys_.end()) {
        return false;
    }

    DeviceInfo deviceInfo = DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetDeviceInfo(
        keyEvent->GetDeviceId());
    if (deviceInfo.UniqIsEmpty()) {
        return false;
    }
    std::unordered_set<DeviceTypeEnum> deviceTypeSet = allMonitorKeys_[keyCode];
    DeviceTypeEnum deviceType;
    if (deviceInfo.deviceType == UNKNOWN) {
        /*
         * When the device type is unknown,
         * if keyboard keys are being monitored, handle them as keyboard inputs.
         */
        if (deviceTypeSet.find(GAME_KEY_BOARD) != deviceTypeSet.end()) {
            deviceType = GAME_KEY_BOARD;
        } else {
            return false;
        }
    } else {
        if (deviceTypeSet.find(deviceInfo.deviceType) != deviceTypeSet.end()) {
            deviceType = deviceInfo.deviceType;
        } else {
            return false;
        }
    }
    if (!DeviceIsSupportKeyMapping(deviceType)) {
        return false;
    }
    handleQueue_->submit([keyEvent, deviceType, deviceInfo, this] {
        HandleKeyEvent(keyEvent, deviceType, deviceInfo);
    });
    return true;
}

bool KeyToTouchManager::DispatchPointerEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    // current only handle mouse event
    if (!BaseKeyToTouchHandler::IsMouseLeftButtonEvent(pointerEvent)
        && !BaseKeyToTouchHandler::IsMouseRightButtonEvent(pointerEvent)
        && !BaseKeyToTouchHandler::IsMouseMoveEvent(pointerEvent)) {
        return false;
    }

    std::lock_guard<ffrt::mutex> lock(checkMutex_);
    if (!IsCanEnableKeyMapping()) {
        return false;
    }
    if (!DeviceIsSupportKeyMapping(GAME_KEY_BOARD)) {
        return false;
    }
    if (!isMonitorMouse_) {
        return false;
    }

    handleQueue_->submit([pointerEvent, this] {
        HandlePointerEvent(pointerEvent, GAME_MOUSE);
    });
    return true;
}

void KeyToTouchManager::UpdateTemplateConfig(const DeviceTypeEnum &deviceType,
                                             const std::vector<KeyToTouchMappingInfo> &mappingInfos)
{
    handleQueue_->submit([deviceType, mappingInfos, this] {
        HandleTemplateConfig(deviceType, mappingInfos);
    });
}

void KeyToTouchManager::UpdateWindowInfo(const WindowInfoEntity &windowInfoEntity)
{
    handleQueue_->submit([windowInfoEntity, this] {
        HandleWindowInfo(windowInfoEntity);
    });
}

void KeyToTouchManager::HandleKeyEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                       const DeviceTypeEnum &deviceType,
                                       const DeviceInfo &deviceInfo)
{
    if (!isEnableKeyMapping_) {
        Rosen::WindowInputInterceptClient::SendInputEvent(keyEvent);
        return;
    }
    std::shared_ptr<InputToTouchContext> context = nullptr;
    if (deviceType == GAME_KEY_BOARD) {
        context = gcKeyboardContext_;
    } else if (deviceType == HOVER_TOUCH_PAD) {
        context = hoverTouchPadContext_;
    }
    if (context == nullptr) {
        Rosen::WindowInputInterceptClient::SendInputEvent(keyEvent);
        return;
    }
    KeyToTouchMappingInfo keyToTouchMappingInfo;
    bool isSuccess = GetMappingInfoByKeyCode(context, keyEvent, deviceInfo, keyToTouchMappingInfo);
    if (!isSuccess) {
        return;
    }
    ExecuteHandle(context, keyToTouchMappingInfo, keyEvent, deviceInfo);
}

void KeyToTouchManager::HandlePointerEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                           const DeviceTypeEnum &deviceType)
{
    if (!isEnableKeyMapping_) {
        Rosen::WindowInputInterceptClient::SendInputEvent(pointerEvent);
        return;
    }
    if (!isMonitorMouse_ || gcKeyboardContext_ == nullptr) {
        Rosen::WindowInputInterceptClient::SendInputEvent(pointerEvent);
        return;
    }

    if (IsHandleMouseMove(gcKeyboardContext_, pointerEvent) ||
        IsHandleMouseRightButtonEvent(gcKeyboardContext_, pointerEvent) ||
        IsHandleMouseLeftButtonEvent(gcKeyboardContext_, pointerEvent)) {
        return;
    }
    Rosen::WindowInputInterceptClient::SendInputEvent(pointerEvent);
}

void KeyToTouchManager::HandleTemplateConfig(const DeviceTypeEnum &deviceType,
                                             const std::vector<KeyToTouchMappingInfo> &mappingInfos)
{
    HILOGI("HandleTemplateConfig deviceType is [%{public}d], mappingInfos size is [%{public}d]",
           deviceType, static_cast<int32_t>(mappingInfos.size()));
    for (const auto &mappingInfo: mappingInfos) {
        HILOGI("mappingInfos [%{public}s]", mappingInfo.GetKeyToTouchMappingInfoDesc().c_str());
    }
    if (deviceType == GAME_KEY_BOARD) {
        InitGcKeyboardContext(mappingInfos);
    } else if (deviceType == HOVER_TOUCH_PAD) {
        InitHoverTouchPadContext(mappingInfos);
    }
    ResetMonitor();
}

void KeyToTouchManager::HandleWindowInfo(const WindowInfoEntity &windowInfoEntity)
{
    HILOGI("windowInfo is [%{public}s]", windowInfoEntity.ToString().c_str());
    windowInfoEntity_ = windowInfoEntity;
    UpdateContextWindowInfo(gcKeyboardContext_);
    UpdateContextWindowInfo(hoverTouchPadContext_);
}

void KeyToTouchManager::UpdateContextWindowInfo(const std::shared_ptr<InputToTouchContext> &context)
{
    if (context == nullptr) {
        return;
    }
    context->windowInfoEntity = windowInfoEntity_;
}

void KeyToTouchManager::InitGcKeyboardContext(const std::vector<KeyToTouchMappingInfo> &mappingInfos)
{
    if (gcKeyboardContext_ != nullptr) {
        DelayedSingleton<MouseRightKeyWalkingDelayHandleTask>::GetInstance()->CancelDelayHandle();
        DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->StopTask();
        ReleaseContext(gcKeyboardContext_);
    }
    gcKeyboardContext_ = std::make_shared<InputToTouchContext>(GAME_KEY_BOARD,
                                                               windowInfoEntity_, mappingInfos);
}

void KeyToTouchManager::InitHoverTouchPadContext(const std::vector<KeyToTouchMappingInfo> &mappingInfos)
{
    if (hoverTouchPadContext_ != nullptr) {
        ReleaseContext(hoverTouchPadContext_);
    }
    if (mappingInfos.empty()) {
        hoverTouchPadContext_ = nullptr;
        return;
    }
    hoverTouchPadContext_ = std::make_shared<InputToTouchContext>(HOVER_TOUCH_PAD,
                                                                  windowInfoEntity_, mappingInfos);
}

void KeyToTouchManager::ReleaseContext(const std::shared_ptr<InputToTouchContext> &inputToTouchContext)
{
    if (!inputToTouchContext->pointerItems.empty()) {
        // send pointer up event for all pointers
        PointerEvent::PointerItem pointerItem;
        for (auto &pointer: inputToTouchContext->pointerItems) {
            std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
            if (pointerEvent == nullptr) {
                HILOGE("Create PointerEvent failed.");
                continue;
            }
            pointerItem = pointer.second;
            pointerEvent->AddPointerItem(pointerItem);
            pointerEvent->SetDeviceId(pointerItem.GetDeviceId());
            pointerEvent->SetActionTime(pointerItem.GetDownTime());
            pointerEvent->SetAgentWindowId(inputToTouchContext->windowInfoEntity.windowId);
            pointerEvent->SetTargetWindowId(inputToTouchContext->windowInfoEntity.windowId);

            pointerEvent->SetId(std::numeric_limits<int32_t>::max());
            pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
            pointerEvent->SetPointerId(pointerItem.GetPointerId());
            pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
            HILOGI("ReleaseContext pointer is [%{public}s].", pointerEvent->ToString().c_str());
            Rosen::WindowInputInterceptClient::SendInputEvent(pointerEvent);
            DelayedSingleton<PointerManager>::GetInstance()->ReleasePointerId(pointerItem.GetPointerId());
        }
    }
    if (inputToTouchContext->isEnterCrosshairInfo) {
        if (mappingHandler_.find(MappingTypeEnum::CROSSHAIR_KEY_TO_TOUCH) != mappingHandler_.end()) {
            mappingHandler_[MappingTypeEnum::CROSSHAIR_KEY_TO_TOUCH]->ExitCrosshairKeyStatus();
        }
    }
}

void KeyToTouchManager::ResetMonitor()
{
    std::lock_guard<ffrt::mutex> lock(checkMutex_);
    allMonitorKeys_.clear();
    isMonitorMouse_ = false;
    ResetAllMonitorKeysAndMouseMonitor(gcKeyboardContext_);
    ResetAllMonitorKeysAndMouseMonitor(hoverTouchPadContext_);
}

void KeyToTouchManager::ResetAllMonitorKeysAndMouseMonitor(const std::shared_ptr<InputToTouchContext> &context)
{
    if (context == nullptr) {
        return;
    }

    if (context->isMonitorMouse) {
        isMonitorMouse_ = true;
    }
    AddToMonitorKeys(context->deviceType, context->singleKeyMappings);
    AddToMonitorKeys(context->deviceType, context->mouseBtnKeyMappings);
    for (const auto &combinationKeyMapping: context->combinationKeyMappings) {
        AddToMonitorKeys(context->deviceType, combinationKeyMapping.first);
    }
}

void KeyToTouchManager::AddToMonitorKeys(const DeviceTypeEnum &deviceType,
                                         const std::unordered_map<int32_t, KeyToTouchMappingInfo> &keyMap)
{
    for (const auto &pair: keyMap) {
        AddToMonitorKeys(deviceType, pair.first);
    }
}

void KeyToTouchManager::AddToMonitorKeys(const DeviceTypeEnum &deviceType,
                                         const int32_t &keyCode)
{
    if (allMonitorKeys_.find(keyCode) == allMonitorKeys_.end()) {
        std::unordered_set<DeviceTypeEnum> deviceTypeSet;
        deviceTypeSet.insert(deviceType);
        allMonitorKeys_[keyCode] = deviceTypeSet;
    } else {
        std::unordered_set<DeviceTypeEnum> deviceTypeSet = allMonitorKeys_[keyCode];
        deviceTypeSet.insert(deviceType);
        allMonitorKeys_[keyCode] = deviceTypeSet;
    }
}

bool KeyToTouchManager::GetMappingInfoByKeyCode(const std::shared_ptr<InputToTouchContext> &context,
                                                const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                const DeviceInfo &deviceInfo,
                                                KeyToTouchMappingInfo &keyToTouchMappingInfo)
{
    if (keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_DOWN) {
        return GetMappingInfoByKeyCodeWhenKeyDown(context, keyEvent, deviceInfo, keyToTouchMappingInfo);
    } else if (BaseKeyToTouchHandler::IsKeyUpEvent(keyEvent)) {
        return GetMappingInfoByKeyCodeWhenKeyUp(context, keyEvent, keyToTouchMappingInfo);
    } else {
        HILOGE("unknown KeyAction [%{public}d].", keyEvent->GetKeyAction());
        return false;
    }
}

bool KeyToTouchManager::GetMappingInfoByKeyCodeWhenKeyDown(const std::shared_ptr<InputToTouchContext> &context,
                                                           const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                           const DeviceInfo &deviceInfo,
                                                           KeyToTouchMappingInfo &keyToTouchMappingInfo)
{
    int32_t keycode = keyEvent->GetKeyCode();

    /**
     * It's key up event.
     * If the current state is combinationKeyOperating and the current key pressed is the last key of the combination,
     * then keyToTouchMappingInfo is context->currentCombinationKey, otherwise obtain keyToTouchMappingInfo from
     * context->singleKeyMappings
     */
    if (context->isCombinationKeyOperating) {
        if (context->currentCombinationKey.combinationKeys.size() != MAX_COMBINATION_KEYS) {
            return false;
        }
        if (context->currentCombinationKey.combinationKeys[COMBINATION_LAST_KEYCODE_IDX] == keycode) {
            keyToTouchMappingInfo = context->currentCombinationKey;
            return true;
        }
    } else {
        if (GetMappingInfoByKeyCodeFromCombinationKey(context, keyEvent, deviceInfo, keyToTouchMappingInfo)) {
            return true;
        }
    }

    if (context->singleKeyMappings.find(keycode) != context->singleKeyMappings.end()) {
        keyToTouchMappingInfo = context->singleKeyMappings[keycode];
        return true;
    }
    return false;
}

bool KeyToTouchManager::GetMappingInfoByKeyCodeWhenKeyUp(const std::shared_ptr<InputToTouchContext> &context,
                                                         const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                         KeyToTouchMappingInfo &keyToTouchMappingInfo)
{
    int32_t keycode = keyEvent->GetKeyCode();
    if (context->isCombinationKeyOperating) {
        /**
         * If the current state is combinationKeyOperating
         * and the current key is a combination key,
         * then keyToTouchMappingInfo is context->currentCombinationKey,
         * otherwise obtain keyToTouchMappingInfo from context->singleKeyMappings
         */
        if (context->currentCombinationKey.combinationKeys.size() != MAX_COMBINATION_KEYS) {
            return false;
        }
        if (context->currentCombinationKey.combinationKeys[COMBINATION_LAST_KEYCODE_IDX] == keycode) {
            keyToTouchMappingInfo = context->currentCombinationKey;
            return true;
        }

        if (context->singleKeyMappings.find(keycode) != context->singleKeyMappings.end()) {
            keyToTouchMappingInfo = context->singleKeyMappings[keycode];
            return true;
        }
        return false;
    }

    if (context->singleKeyMappings.find(keycode) != context->singleKeyMappings.end()) {
        keyToTouchMappingInfo = context->singleKeyMappings[keycode];
        return true;
    }
    return false;
}

bool KeyToTouchManager::GetMappingInfoByKeyCodeFromCombinationKey(const std::shared_ptr<InputToTouchContext> &context,
                                                                  const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                                  const DeviceInfo &deviceInfo,
                                                                  KeyToTouchMappingInfo &keyToTouchMappingInfo)
{
    int32_t keycode = keyEvent->GetKeyCode();
    if (context->combinationKeyMappings.find(keycode) == context->combinationKeyMappings.end()) {
        return false;
    }
    int64_t keyDownTime = keyEvent->GetActionTime();
    int32_t deviceId = keyEvent->GetDeviceId();
    bool isCombinationKey = false;
    int64_t combinationKeyDownTime = 0;
    std::unordered_map<int32_t, KeyToTouchMappingInfo> combinationMap = context->combinationKeyMappings[keycode];
    for (auto &keyItem: keyEvent->GetKeyItems()) {
        if (keyItem.GetDeviceId() == deviceId
            && keyItem.GetDownTime() >= deviceInfo.onlineTime
            && keyItem.GetDownTime() < keyDownTime
            && combinationMap.find(keyItem.GetKeyCode()) != combinationMap.end()) {
            isCombinationKey = true;
            if (combinationKeyDownTime == 0 || keyItem.GetDownTime() < combinationKeyDownTime) {
                /**
                 * for example, the combinationKey can be shift+B or alt+B,
                 * If we press Shift, Alt, and B in sequence,
                 * we will select Shift+B as the combination key.
                 */
                combinationKeyDownTime = keyItem.GetDownTime();
                keyToTouchMappingInfo = combinationMap[keyItem.GetKeyCode()];
                continue;
            }
        }
    }
    if (isCombinationKey) {
        return true;
    }
    return false;
}

void KeyToTouchManager::ExecuteHandle(std::shared_ptr<InputToTouchContext> &context,
                                      const KeyToTouchMappingInfo &mappingInfo,
                                      const std::shared_ptr<MMI::KeyEvent> &keyEvent, const DeviceInfo &deviceInfo)
{
    if (mappingHandler_.find(mappingInfo.mappingType) == mappingHandler_.end()) {
        return;
    }
    mappingHandler_[mappingInfo.mappingType]->HandleKeyEvent(context, keyEvent,
                                                             deviceInfo, mappingInfo);
}

void KeyToTouchManager::ExecuteHandle(std::shared_ptr<InputToTouchContext> &context,
                                      const KeyToTouchMappingInfo &mappingInfo,
                                      const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (mappingHandler_.find(mappingInfo.mappingType) == mappingHandler_.end()) {
        HILOGE("cannot find mappingHandler [%{public}d]", mappingInfo.mappingType);
        return;
    }
    mappingHandler_[mappingInfo.mappingType]->HandlePointerEvent(context, pointerEvent, mappingInfo);
}

bool KeyToTouchManager::IsHandleMouseMove(std::shared_ptr<InputToTouchContext> &context,
                                          const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (!BaseKeyToTouchHandler::IsMouseMoveEvent(pointerEvent)) {
        return false;
    }

    bool isNeedHandle = false;
    if (context->isSkillOperating) {
        mappingHandler_[SKILL_KEY_TO_TOUCH]->HandlePointerEvent(context, pointerEvent,
                                                                context->currentSkillKeyInfo);
        isNeedHandle = true;
    }

    if (context->isCrosshairMode) {
        if (context->isEnterCrosshairInfo) {
            ExecuteHandle(context, context->currentCrosshairInfo, pointerEvent);
            isNeedHandle = true;
        }
    }

    if (context->IsMouseRightWalking()) {
        if (context->mouseBtnKeyMappings.find(MOUSE_RIGHT_BUTTON_KEYCODE) != context->mouseBtnKeyMappings.end()) {
            ExecuteHandle(context, context->mouseBtnKeyMappings[MOUSE_RIGHT_BUTTON_KEYCODE],
                          pointerEvent);
            isNeedHandle = true;
        }
    }

    if (context->isPerspectiveObserving &&
        (context->currentPerspectiveObserving.mappingType == OBSERVATION_KEY_TO_TOUCH ||
            context->currentPerspectiveObserving.mappingType == MOUSE_OBSERVATION_TO_TOUCH)) {
        ExecuteHandle(context, context->currentPerspectiveObserving, pointerEvent);
        isNeedHandle = true;
    }
    return isNeedHandle;
}

bool KeyToTouchManager::IsHandleMouseRightButtonEvent(std::shared_ptr<InputToTouchContext> &context,
                                                      const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (!BaseKeyToTouchHandler::IsMouseRightButtonEvent(pointerEvent)) {
        return false;
    }

    if (context->mouseBtnKeyMappings.find(MOUSE_RIGHT_BUTTON_KEYCODE) != context->mouseBtnKeyMappings.end()) {
        ExecuteHandle(context, context->mouseBtnKeyMappings[MOUSE_RIGHT_BUTTON_KEYCODE], pointerEvent);
        return true;
    }
    return false;
}

bool KeyToTouchManager::IsHandleMouseLeftButtonEvent(std::shared_ptr<InputToTouchContext> &context,
                                                     const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    if (!BaseKeyToTouchHandler::IsMouseLeftButtonEvent(pointerEvent)) {
        return false;
    }

    if (context->isCrosshairMode
        && context->mouseBtnKeyMappings.find(MOUSE_LEFT_BUTTON_KEYCODE) != context->mouseBtnKeyMappings.end()) {
        // Perform mouse left button events in crosshair-mode
        ExecuteHandle(context, context->mouseBtnKeyMappings[MOUSE_LEFT_BUTTON_KEYCODE], pointerEvent);
        return true;
    }

    if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_BUTTON_UP
        && context->isMouseLeftFireOperating
        && context->mouseBtnKeyMappings.find(MOUSE_LEFT_BUTTON_KEYCODE) != context->mouseBtnKeyMappings.end()) {
        /*
         * When it's not in crosshair-mode, if it is a up event and the mouse leftButton was pressed,
         * perform mouse left button events.
         */
        ExecuteHandle(context, context->mouseBtnKeyMappings[MOUSE_LEFT_BUTTON_KEYCODE], pointerEvent);
        return true;
    }
    return false;
}

void KeyToTouchManager::EnableKeyMapping(bool isEnable)
{
    handleQueue_->submit([isEnable, this] {
        HandleEnableKeyMapping(isEnable);
    });
}

void KeyToTouchManager::HandleEnableKeyMapping(bool isEnable)
{
    std::lock_guard<ffrt::mutex> lock(checkMutex_);
    isEnableKeyMapping_ = isEnable;
    HILOGI("EnableKeyMapping([%{public}d]). 1 is enable", isEnable ? 1 : 0);
    ResetContext(gcKeyboardContext_);
    ResetContext(hoverTouchPadContext_);
}

void KeyToTouchManager::ResetContext(std::shared_ptr<InputToTouchContext> &context)
{
    if (context == nullptr) {
        return;
    }
    ReleaseContext(context);
    context->ResetTempVariables();
}

bool KeyToTouchManager::IsCanEnableKeyMapping()
{
    return isSupportKeyMapping_ && isEnableKeyMapping_ && windowInfoEntity_.isFullScreen;
}

bool KeyToTouchManager::DeviceIsSupportKeyMapping(DeviceTypeEnum deviceTypeEnum)
{
    if (supportDeviceTypeSet_.empty()) {
        // Compatibility processing, older versions does not have supportDeviceType
        return true;
    }
    if (supportDeviceTypeSet_.count(deviceTypeEnum) == 0) {
        return false;
    }
    return true;
}

void KeyToTouchManager::UpdateByDeviceStatusChanged(const DeviceInfo &deviceInfo)
{
    if (deviceInfo.status == 0
        && deviceInfo.sourceTypeSet.count(MOUSE) != 0) {
        handleQueue_->submit([this] {
            if (gcKeyboardContext_ == nullptr) {
                return;
            }

            if (gcKeyboardContext_->isEnterCrosshairInfo) {
                InputManager::GetInstance()->SetPointerVisible(false, 0);
            }
        });
    }
}

void KeyToTouchManager::CheckPointerSendInterval()
{
    if (gcKeyboardContext_ != nullptr) {
        gcKeyboardContext_->CheckPointerSendInterval();
    }
    handleQueue_->submit([this] {
        CheckPointerSendInterval();
    }, ffrt::task_attr().name("pointer-check-task").delay(DELAY_TIME_UNIT));
}
}
}