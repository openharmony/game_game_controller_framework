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
#include <cstring>
#include <dlfcn.h>
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
#include "plugin_callback_manager.h"

namespace OHOS {
namespace GameController {
namespace {
constexpr const char* PERMISSION = "ohos.permission.PUBLISH_SYSTEM_COMMON_EVENT";
constexpr const char* KEY_MAPPING_CHANGE_EVENT = "usual.event.ohos.gamecontroller.game.keymapping.change";
constexpr const char* KEY_MAPPING_ENABLE_EVENT = "usual.event.ohos.gamecontroller.game.keymapping.enable";
constexpr const char* SUPPORT_KEY_MAPPING_CHANGE_EVENT =
    "usual.event.ohos.gamecontroller.supported.keymapping.change";
constexpr const char* SCB_FORWARD_KEY_EVENT = "custom.event.SCB_FORWARD_KEYEVENT";
constexpr const char* SCB_BUNDLE_NAME = "com.ohos.sceneboard";
constexpr const char* EVENT_PARAM_BUNDLE_NAME = "bundleName";
constexpr const char* EVENT_PARAM_DEVICE_TYPE = "deviceType";
constexpr const char* EVENT_PARAM_ENABLE = "enable";
constexpr const char* EVENT_PARAM_KEYCODE = "keyCode";
const int32_t GAME_CONTROLLER_UID = 6227;
const std::string TV_DEVICE_TYPE = "tv";
const int KEY_MAPPING_ENABLE = 1;
const int KEYCODE_OPEN_TEMP_FOR_HOVER_TOUCH_CONTROLLER = 3107;
const char* PLUGIN_LIB_PATH = "/system/lib64/libgamecontroller_anco_plugin.z.so";
static BundleBasicInfo g_bundleInfo;
static bool g_isPluginMode = false;
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
        StartPluginMode();
        return;
    }

    // Determine whether support key mapping configurations.
    if (!IsSupportKeyMapping()) {
        HILOGI("The app does not support input-to-touch feature.");
        return;
    }

    HILOGI("The app supports input-to-touch feature.");
    DelayedSingleton<KeyMappingHandle>::GetInstance()->SetSupportKeyMapping(true);

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
    if (!g_isPluginMode) {
        DelayedSingleton<KeyToTouchManager>::GetInstance()->SetCurrentBundleName(g_bundleInfo.bundleName, true, false);
        DelayedSingleton<WindowInfoManager>::GetInstance()->InitWindowInfo(g_bundleInfo.bundleName);
        DelayedSingleton<KeyMappingService>::GetInstance()->SetWindowId(
            DelayedSingleton<WindowInfoManager>::GetInstance()->GetWindowId());
    }
    DelayedSingleton<PluginCallbackManager>::GetInstance()->SetPluginMode(g_isPluginMode);
    DelayedSingleton<MultiModalInputMonitor>::GetInstance()->RegisterMonitorBySystem();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetAllDeviceInfosWhenRegisterDeviceMonitor();
}

void InputToTouchClient::StartPublicEventMonitor()
{
    SubscribeGameControllerSaEvent();

    if (OHOS::system::GetDeviceType() == TV_DEVICE_TYPE) {
        SubscribeScbEvent();
    }
}

void InputToTouchClient::SubscribeGameControllerSaEvent()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(KEY_MAPPING_CHANGE_EVENT);
    matchingSkills.AddEvent(KEY_MAPPING_ENABLE_EVENT);
    matchingSkills.AddEvent(SUPPORT_KEY_MAPPING_CHANGE_EVENT);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPermission(PERMISSION);
    subscribeInfo.SetPublisherUid(GAME_CONTROLLER_UID);

    subscriber_ = std::make_shared<GameCommonEventListener>(subscribeInfo);
    if (EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_)) {
        HILOGI("SubscribeGameControllerCommonEvent success");
    } else {
        HILOGE("SubscribeGameControllerCommonEvent failed");
    }
}

void InputToTouchClient::SubscribeScbEvent()
{
    EventFwk::MatchingSkills scbMatchingSkills;
    scbMatchingSkills.AddEvent(SCB_FORWARD_KEY_EVENT);
    EventFwk::CommonEventSubscribeInfo subscribeInfoForScb(scbMatchingSkills);
    subscribeInfoForScb.SetPublisherBundleName(SCB_BUNDLE_NAME);
    subscriberForScb_ = std::make_shared<GameCommonEventListener>(subscribeInfoForScb);
    if (EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberForScb_)) {
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

    if (action == SUPPORT_KEY_MAPPING_CHANGE_EVENT) {
        HandleSupportedKeyMappingChangeEvent();
        return;
    }
    std::string bundleName = want.GetStringParam(EVENT_PARAM_BUNDLE_NAME);
    if (!IsCurrentGameEvent(bundleName)) {
        return;
    }
    if (action == KEY_MAPPING_CHANGE_EVENT) {
        HandleTemplateChangeEvent(bundleName, data);
    } else if (action == KEY_MAPPING_ENABLE_EVENT) {
        HandleKeyMappingEnableChangeEvent(bundleName, data);
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
        bundleName, static_cast<DeviceTypeEnum>(deviceType));
}

void GameCommonEventListener::HandleKeyMappingEnableChangeEvent(const std::string &bundleName,
                                                                const EventFwk::CommonEventData &data)
{
    AAFwk::Want want = data.GetWant();
    int enable = want.GetIntParam(EVENT_PARAM_ENABLE, KEY_MAPPING_ENABLE);
    DelayedSingleton<KeyToTouchManager>::GetInstance()->EnableKeyMapping(bundleName,
                                                                         enable);
    DelayedSingleton<PluginCallbackManager>::GetInstance()->HandleKeyMappingEnableChangeEvent(bundleName, enable);
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
        ->GetOneDeviceByDeviceType(HOVER_TOUCH_PAD);
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

void GameCommonEventListener::HandleSupportedKeyMappingChangeEvent()
{
    DelayedSingleton<KeyMappingService>::GetInstance()->ReloadKeyMappingSupportConfig();
}

bool GameCommonEventListener::IsCurrentGameEvent(const std::string &bundleName)
{
    if (g_isPluginMode) {
        return true;
    }
    if (bundleName != g_bundleInfo.bundleName) {
        HILOGW("no need handle bundleName[%{public}s]'s public event. the current bundleName[%{public}s]'s",
               bundleName.c_str(),
               g_bundleInfo.bundleName.c_str());
        return false;
    }
    return true;
}

static std::unique_ptr<void, void (*)(void*)> pluginHandle_{nullptr, [](void* handle) {
    if (handle) {
        dlclose(handle);
        HILOGW("close plugin so");
    }
}};

void InputToTouchClient::StartPluginMode()
{
    if (!DelayedSingleton<KeyMappingService>::GetInstance()->CheckSupportKeyMapping(g_bundleInfo.bundleName)) {
        return;
    }

    HILOGW("the system bundleName[%{public}s] support by GameControllerAncoPlugin", g_bundleInfo.bundleName.c_str());
    void* handle = dlopen(PLUGIN_LIB_PATH, RTLD_LAZY | RTLD_GLOBAL);
    if (handle) {
        HILOGW("dlopen GameControllerAncoPlugin success");
        pluginHandle_.reset(handle);
    } else {
        HILOGW("dlopen %{public}s failed", dlerror());
        return;
    }
    DelayedSingleton<KeyMappingHandle>::GetInstance()->SetSupportKeyMapping(true);
    g_isPluginMode = true;
    StartInputMonitor();
    StartPublicEventMonitor();
}

}
}