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
#include "input_to_touch_client.h"
#include "gamecontroller_log.h"
#include "multi_modal_input_monitor.h"
#include "key_mapping_service.h"
#include "key_mapping_handle.h"
#include "bundle_manager.h"
#include "multi_modal_input_mgt_service.h"
#include "gamecontroller_utils.h"

namespace OHOS {
namespace GameController {
namespace {
constexpr const char* PERMISSION = "ohos.permission.PUBLISH_SYSTEM_COMMON_EVENT";
constexpr const char* KEY_MAPPING_CHANGE_EVENT = "usual.event.ohos.gamecontroller.game.keymapping.change";
constexpr const char* EVENT_PARAM_BUNDLE_NAME = "bundleName";
constexpr const char* EVENT_PARAM_DEVICE_TYPE = "deviceType";

const int32_t GAME_CONTROLLER_UID = 6227;
static BundleBasicInfo g_bundleInfo;
}

std::shared_ptr<GameCommonEventListener> InputToTouchClient::subscriber_ = nullptr;

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
    DelayedSingleton<MultiModalInputMonitor>::GetInstance()->RegisterMonitorBySystem();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetAllDeviceInfosWhenRegisterDeviceMonitor();
}

void InputToTouchClient::StartPublicEventMonitor()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(KEY_MAPPING_CHANGE_EVENT);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPermission(PERMISSION);
    subscribeInfo.SetPublisherUid(GAME_CONTROLLER_UID);

    InputToTouchClient::subscriber_ = std::make_shared<GameCommonEventListener>(subscribeInfo);

    if (EventFwk::CommonEventManager::SubscribeCommonEvent(InputToTouchClient::subscriber_)) {
        HILOGI("SubscribeCommonEvent success");
    } else {
        HILOGE("SubscribeCommonEvent failed");
    }
}

void GameCommonEventListener::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    AAFwk::Want want = data.GetWant();
    std::string bundleName = want.GetStringParam(EVENT_PARAM_BUNDLE_NAME);
    if (bundleName != g_bundleInfo.bundleName) {
        HILOGW("no need handle bundleName[%{public}s]'s public event. the current bundleName[%{public}s]'s",
               bundleName.c_str(),
               g_bundleInfo.bundleName.c_str());
        return;
    }

    int deviceType = want.GetIntParam(EVENT_PARAM_DEVICE_TYPE, 0);
    DeviceInfo deviceInfo;
    deviceInfo.deviceType = static_cast<DeviceTypeEnum>(deviceType);
    HILOGI("Get the keymapping info by bundleName:%{public}s, deviceType:%{public}d", bundleName.c_str(),
           deviceType);
    DelayedSingleton<KeyMappingService>::GetInstance()->GetGameKeyMappingFromSa(deviceInfo, false);
}
}
}