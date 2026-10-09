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

#include "shortcut_key_handler.h"
#include <common_event_manager.h>
#include <common_event_publish_info.h>
#include "gamecontroller_log.h"
#include <want.h>
#include <thread>

namespace OHOS {
namespace GameController {

namespace {
const char* SHORTCUTKEY_STATUS_EVENT = "usual.event.ohos.gamecontroller.game.keymapping.shortcutkey.status";
const char* KEYMAPPING_RESET_EVENT = "usual.event.ohos.gamecontroller.game.keymapping.reset";
const char* EVENT_PARAM_KEYCODE = "keycode";
const char* EVENT_PARAM_STATUS = "status";
const char* EVENT_PARAM_FEATURE = "feature";
const int32_t FEATURE_SHORTCUT_KEY = 0;
}

void ShortcutKeyHandler::HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                                       const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                       const KeyToTouchMappingInfo &mappingInfo,
                                       const DeviceInfo &deviceInfo)
{
    HILOGI("[SHORTCUT_HANDLER]HandleKeyDown, keyCode[%{public}d]", keyEvent->GetKeyCode());
}

void ShortcutKeyHandler::HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                                     const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                     const DeviceInfo &deviceInfo)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    HILOGI("[SHORTCUT_HANDLER]HandleKeyUp, keyCode[%{public}d], send status event to gtx", keyCode);
    
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(false);
    AAFwk::Want want;
    want.SetAction(SHORTCUTKEY_STATUS_EVENT);
    want.SetParam(EVENT_PARAM_KEYCODE, keyCode);
    want.SetParam(EVENT_PARAM_STATUS, 1);
    want.SetParam(EVENT_PARAM_FEATURE, FEATURE_SHORTCUT_KEY);
    EventFwk::CommonEventData event;
    event.SetWant(want);
    EventFwk::CommonEventManager::NewPublishCommonEvent(event, publishInfo);
}

void ShortcutKeyHandler::SendKeyMappingResetNotify()
{
    HILOGI("[SHORTCUT_HANDLER]SendKeyMappingResetNotify");
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(false);
    AAFwk::Want want;
    want.SetAction(KEYMAPPING_RESET_EVENT);
    EventFwk::CommonEventData event;
    event.SetWant(want);
    EventFwk::CommonEventManager::NewPublishCommonEvent(event, publishInfo);
    HILOGI("SendKeyMappingResetNotify: Event published successfully");
}

}
}
