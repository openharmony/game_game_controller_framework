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

#include <thread>
#include <common_event_manager.h>
#include <matching_skills.h>
#include <syspara/parameters.h>
#include "input_to_touch_client.h"
#include "gamecontroller_log.h"
#include "multi_modal_input_monitor.h"
#include "key_mapping_service.h"
#include "key_mapping_handle.h"
#include "bundle_manager.h"
#include "multi_modal_input_mgt_service.h"
#include "gamecontroller_utils.h"
#include "window_info_manager.h"
#include "key_to_touch_manager.h"

namespace OHOS {
namespace GameController {
namespace {
constexpr const char* PERMISSION = "ohos.permission.PUBLISH_SYSTEM_COMMON_EVENT";
constexpr const char* KEY_MAPPING_CHANGE_EVENT = "usual.event.ohos.gamecontroller.game.keymapping.change";
constexpr const char* KEY_MAPPING_ENABLE_EVENT = "usual.event.ohos.gamecontroller.game.keymapping.enable";
constexpr const char* SCB_FORWARD_KEY_EVENT = "custom.event.SCB_FORWARD_KEYEVENT";
constexpr const char* SCB_BUNDLE_NAME = "com.ohos.sceneboard";
constexpr const char* EVENT_PARAM_BUNDLE_NAME = "bundleName";
constexpr const char* EVENT_PARAM_DEVICE_TYPE = "deviceType";
constexpr const char* EVENT_PARAM_ENABLE = "enable";
constexpr const char* EVENT_PARAM_KEYCODE = "keyCode";
const int32_t GAME_CONTROLLER_UID = 6227;
static BundleBasicInfo g_bundleInfo;
const std::string PC_DEVICE_TYPE = "2in1";
const int KEY_MAPPING_ENABLE = 1;
const int KEYCODE_OPEN_TEMP_FOR_HOVER_TOUCH_CONTROLLER = 3107;
}

std::shared_ptr<GameCommonEventListener> InputToTouchClient::subscriber_ = nullptr;
std::shared_ptr<GameCommonEventListener> InputToTouchClient::subscriberForScb_ = nullptr;

void InputToTouchClient::StartInputToTouch()
{
    HILOGI("Begin StartInputToTouch");
    std::thread th(DoAsyncTask);
    th.detach();
}

void InputToTouchClient::DoAsyncTask()
{
    // Determine whether the app is a common app.
    if (!IsCommonApp()) {
        return;
    }

    // Determine whether support key mapping configurations.
    if (!IsSupportKeyMapping()) {
        HILOGI("The app does not support input-to-touch feature.");
        return;
    }
    DelayedSingleton<KeyMappingHandle>::GetInstance()->SetSupportKeyMapping(true);
    DelayedSingleton<KeyToTouchManager>::GetInstance()->SetSupportKeyMapping(true);
    HILOGI("The app supports input-to-touch feature.");

    // Start Multi-Modal-Input Monitor
    StartInputMonitor();

    // Start public event monitor.
    StartPublicEventMonitor();
}

bool InputToTouchClient::IsCommonApp()
{
    g_bundleInfo = DelayedSingleton<BundleManager>::GetInstance()->GetSelfBundleName();
    if (g_bundleInfo.IsCommonApp()) {
        HILOGI("The [%{public}s] is a common app.", g_bundleInfo.bundleName.c_str());
        return true;
    }
    HILOGI("The [%{public}s] is not a common app.", g_bundleInfo.bundleName.c_str());
    return false;
}

bool InputToTouchClient::IsSupportKeyMapping()
{
    return DelayedSingleton<KeyMappingService>::GetInstance()->IsSupportGameKeyMapping(g_bundleInfo.bundleName,
                                                                                       g_bundleInfo.version);
}

void InputToTouchClient::StartInputMonitor()
{
    DelayedSingleton<WindowInfoManager>::GetInstance()->InitWindowInfo(g_bundleInfo.bundleName);
    DelayedSingleton<MultiModalInputMonitor>::GetInstance()->RegisterMonitorBySystem();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetAllDeviceInfosWhenRegisterDeviceMonitor();
    // If it's a PC, simulate the built-in keyboard online.
    if (OHOS::system::GetDeviceType() == PC_DEVICE_TYPE) {
        HILOGI("It's pc device.");
        g_bundleInfo.isPC = true;
        DelayedSingleton<KeyMappingHandle>::GetInstance()->SetIsPC(true);
    }
}

void InputToTouchClient::StartPublicEventMonitor()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(KEY_MAPPING_CHANGE_EVENT);
    matchingSkills.AddEvent(KEY_MAPPING_ENABLE_EVENT);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPermission(PERMISSION);
    subscribeInfo.SetPublisherUid(GAME_CONTROLLER_UID);

    InputToTouchClient::subscriber_ = std::make_shared<GameCommonEventListener>(subscribeInfo);

    if (EventFwk::CommonEventManager::SubscribeCommonEvent(InputToTouchClient::subscriber_)) {
        HILOGI("SubscribeGameControllerCommonEvent success");
    } else {
        HILOGE("SubscribeGameControllerCommonEvent failed");
    }

    EventFwk::MatchingSkills scbMatchingSkills;
    scbMatchingSkills.AddEvent(SCB_FORWARD_KEY_EVENT);
    EventFwk::CommonEventSubscribeInfo subscribeInfoForScb(scbMatchingSkills);
    subscribeInfoForScb.SetPublisherBundleName(SCB_BUNDLE_NAME);
    InputToTouchClient::subscriberForScb_ = std::make_shared<GameCommonEventListener>(subscribeInfoForScb);
    if (EventFwk::CommonEventManager::SubscribeCommonEvent(InputToTouchClient::subscriberForScb_)) {
        HILOGI("SubscribeScbCommonEvent success");
    } else {
        HILOGE("SubscribeScbCommonEvent failed");
    }
}

void GameCommonEventListener::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    AAFwk::Want want = data.GetWant();
    std::string action = want.GetAction();
    if (action == SCB_FORWARD_KEY_EVENT) {
        HandleScbForwardKeyEvent(data);
        return;
    }
    std::string bundleName = want.GetStringParam(EVENT_PARAM_BUNDLE_NAME);
    if (!IsCurrentGameEvent(bundleName)) {
        return;
    }
    if (action == KEY_MAPPING_CHANGE_EVENT) {
        HandleTemplateChangeEvent(bundleName, data);
    } else if (action == KEY_MAPPING_ENABLE_EVENT) {
        HandleKeyMappingEnableChangeEvent(data);
    } else {
        HILOGI("unknown sa event %{public}s", action.c_str());
    }
}

void GameCommonEventListener::HandleTemplateChangeEvent(const std::string &bundleName,
                                                        const EventFwk::CommonEventData &data)
{
    AAFwk::Want want = data.GetWant();
    int deviceType = want.GetIntParam(EVENT_PARAM_DEVICE_TYPE, 0);
    HILOGI("Get the keymapping info by bundleName:%{public}s, deviceType:%{public}d",
           bundleName.c_str(), deviceType);
    DelayedSingleton<KeyMappingService>::GetInstance()->UpdateGameKeyMappingWhenTemplateChange(
        static_cast<DeviceTypeEnum>(deviceType));
}

void GameCommonEventListener::HandleKeyMappingEnableChangeEvent(const EventFwk::CommonEventData &data)
{
    AAFwk::Want want = data.GetWant();
    int enable = want.GetIntParam(EVENT_PARAM_ENABLE, KEY_MAPPING_ENABLE);
    DelayedSingleton<KeyToTouchManager>::GetInstance()->EnableKeyMapping(enable == KEY_MAPPING_ENABLE ? true : false);
}

void GameCommonEventListener::HandleScbForwardKeyEvent(const EventFwk::CommonEventData &data)
{
    AAFwk::Want want = data.GetWant();
    int code = want.GetIntParam(EVENT_PARAM_KEYCODE, 0);
    if (code != KEYCODE_OPEN_TEMP_FOR_HOVER_TOUCH_CONTROLLER) {
        HILOGW("Discard HandleScbForwardKeyEvent. code is %{public}d", code);
        return;
    }

    std::pair<bool, DeviceInfo> deviceInfo = DelayedSingleton<MultiModalInputMgtService>::GetInstance()
        ->GetHoverTouchPad();
    if (!deviceInfo.first) {
        HILOGW("Discard HandleScbForwardKeyEvent. No HoverTouchPad");
        return;
    }

    if (DelayedSingleton<WindowInfoManager>::GetInstance()->IsForegroundAndFocus()) {
        HILOGI("HandleScbForwardKeyEvent. Game is foreground and focus");
        DelayedSingleton<KeyMappingService>::GetInstance()->BroadcastOpenTemplateConfig(deviceInfo.second);
    } else {
        HILOGW("Discard HandleScbForwardKeyEvent. Game is not foreground and focus");
    }
}

bool GameCommonEventListener::IsCurrentGameEvent(const std::string &bundleName)
{
    if (bundleName != g_bundleInfo.bundleName) {
        HILOGW("no need handle bundleName[%{public}s]'s public event. the current bundleName[%{public}s]'s",
               bundleName.c_str(),
               g_bundleInfo.bundleName.c_str());
        return false;
    }
    return true;
}
}
}