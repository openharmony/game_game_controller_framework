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

#include "trigger_to_touch_handler.h"
#include "key_to_touch_manager.h"
#include "plugin_callback_manager.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
void TriggerToTouchHandler::HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                               const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                               const KeyToTouchMappingInfo &mappingInfo)
{
    if (pointerEvent == nullptr || context == nullptr) {
        HILOGW("pointerEvent or context is nullptr");
        return;
    }
    PointerEvent::AxisType axisType = (mappingInfo.joystick == 0)
        ? PointerEvent::AxisType::AXIS_TYPE_ABS_BRAKE
        : PointerEvent::AxisType::AXIS_TYPE_ABS_GAS;
    if (!pointerEvent->HasAxis(axisType)) {
        return;
    }
    double axisVal = pointerEvent->GetAxisValue(axisType);
    if (axisVal > TRIGGER_THRESHOLD && !isActive_) {
        isActive_ = true;
        DelayedSingleton<KeyToTouchManager>::GetInstance()->InjectGamepadTriggerKey(
            context, mappingInfo.keyCode, MMI::KeyEvent::KEY_ACTION_DOWN, pointerEvent->GetActionTime());
        auto bridgeKeyEvent = MMI::KeyEvent::Create();
        bridgeKeyEvent->SetKeyCode(mappingInfo.keyCode);
        bridgeKeyEvent->SetActionTime(pointerEvent->GetActionTime());
        bridgeKeyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
        DelayedSingleton<PluginCallbackManager>::GetInstance()->SendInputEvent(
            context->windowInfoEntity.bundleName, bridgeKeyEvent, false);
        HILOGI("Trigger joystick=%{public}d pressed -> keyCode %{public}d DOWN",
               mappingInfo.joystick, mappingInfo.keyCode);
    } else if (axisVal <= TRIGGER_THRESHOLD && isActive_) {
        isActive_ = false;
        DelayedSingleton<KeyToTouchManager>::GetInstance()->InjectGamepadTriggerKey(
            context, mappingInfo.keyCode, MMI::KeyEvent::KEY_ACTION_UP, pointerEvent->GetActionTime());
        auto bridgeKeyEvent = MMI::KeyEvent::Create();
        bridgeKeyEvent->SetKeyCode(mappingInfo.keyCode);
        bridgeKeyEvent->SetActionTime(pointerEvent->GetActionTime());
        bridgeKeyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
        DelayedSingleton<PluginCallbackManager>::GetInstance()->SendInputEvent(
            context->windowInfoEntity.bundleName, bridgeKeyEvent, false);
        HILOGI("Trigger joystick=%{public}d released -> keyCode %{public}d UP",
               mappingInfo.joystick, mappingInfo.keyCode);
    }
}
}
}