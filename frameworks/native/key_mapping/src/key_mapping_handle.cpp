/*
 *  Copyright (c) 2025 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <display.h>
#include <window_input_intercept_client.h>
#include <input_manager.h>
#include "key_mapping_handle.h"
#include "gamecontroller_log.h"
#include "multi_modal_input_mgt_service.h"
#include "key_mapping_service.h"

using namespace OHOS::MMI;
namespace OHOS {
namespace GameController {
namespace {
const int32_t KEYCODE_OPEN_TEMP_FOR_HOVER_TOUCH_CONTROLLER = 3107;
}

KeyMappingHandle::KeyMappingHandle()
{
}

KeyMappingHandle::~KeyMappingHandle()
{
}

bool KeyMappingHandle::IsNeedKeyToTouch(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    if (!isSupportKeyMapping_) {
        return false;
    }

    // Only the key up and down event needs to be processed.
    if (keyEvent->GetKeyAction() != KeyEvent::KEY_ACTION_UP
        && keyEvent->GetKeyAction() != KeyEvent::KEY_ACTION_DOWN) {
        return false;
    }

    DeviceInfo deviceInfo = DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetDeviceInfo(
        keyEvent->GetDeviceId());
    if (deviceInfo.UniqIsEmpty()) {
        return false;
    }

    if (IsNotifyOpenTemplateConfigPage(keyEvent, deviceInfo)) {
        return false;
    }

    int32_t keyCode = keyEvent->GetKeyCode();
    std::pair<bool, KeyToTouchMappingInfo> keyMappingConfig = DelayedSingleton<KeyMappingService>::GetInstance()
        ->GetGameKeyMappingFromCache(deviceInfo, keyCode);

    // Processed only when the key mapping configuration exists.
    if (!keyMappingConfig.first) {
        HILOGD("key [%{public}d] no keyMappingConfig.", keyCode);
        return false;
    }

    if (keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_DOWN) {
        ChangeKeyEventToPointer(keyEvent, PointerEvent::POINTER_ACTION_DOWN, keyMappingConfig.second);
    }
    if (keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_UP) {
        ChangeKeyEventToPointer(keyEvent, PointerEvent::POINTER_ACTION_UP, keyMappingConfig.second);
    }
    return true;
}

void KeyMappingHandle::ChangeKeyEventToPointer(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                               const int32_t &pointerAction,
                                               const KeyToTouchMappingInfo &keyMappingInfo)
{
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    if (pointerEvent == nullptr) {
        HILOGE("Create PointerEvent failed.");
        return;
    }
    PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    int32_t valX = keyMappingInfo.xValue;
    pointerItem.SetDisplayX(valX);
    pointerItem.SetDisplayXPos(valX);
    pointerItem.SetWindowX(valX);
    pointerItem.SetWindowXPos(valX);
    int32_t valY = keyMappingInfo.yValue;
    pointerItem.SetDisplayY(valY);
    pointerItem.SetDisplayYPos(valY);
    pointerItem.SetWindowY(valY);
    pointerItem.SetWindowYPos(valY);
    pointerItem.SetPressure(1);
    pointerItem.SetDeviceId(1);
    pointerEvent->SetActionTime(keyEvent->GetActionTime());
    pointerEvent->SetAgentWindowId(keyEvent->GetAgentWindowId());
    pointerEvent->SetTargetDisplayId(keyEvent->GetTargetDisplayId());
    pointerEvent->SetTargetWindowId(keyEvent->GetTargetWindowId());
    pointerEvent->AddPointerItem(pointerItem);
    pointerEvent->SetId(std::numeric_limits<int32_t>::max());
    pointerEvent->SetPointerAction(pointerAction);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    HILOGD("keyCode [%{public}d] changeToPointer. pointer is [%{public}s].", keyEvent->GetKeyCode(),
           pointerEvent->ToString().c_str());
    Rosen::WindowInputInterceptClient::SendInputEvent(pointerEvent);
}

void KeyMappingHandle::SetSupportKeyMapping(bool isSupportKeyMapping)
{
    isSupportKeyMapping_ = isSupportKeyMapping;
}

bool KeyMappingHandle::IsNotifyOpenTemplateConfigPage(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                      const DeviceInfo &deviceInfo)
{
    if (keyEvent->GetKeyAction() != KeyEvent::KEY_ACTION_DOWN) {
        return false;
    }

    if (deviceInfo.deviceType != DeviceTypeEnum::HOVER_TOUCH_PAD) {
        return false;
    }

    int32_t keyCode = keyEvent->GetKeyCode();
    if (keyCode == KEYCODE_OPEN_TEMP_FOR_HOVER_TOUCH_CONTROLLER) {
        DelayedSingleton<KeyMappingService>::GetInstance()->BroadcastOpenTemplateConfig(deviceInfo);
        return true;
    }
    return false;
}
}
}