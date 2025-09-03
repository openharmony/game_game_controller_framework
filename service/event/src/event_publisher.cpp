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

#include "event_publisher.h"
#include <common_event_publish_info.h>
#include <want.h>
#include "gamecontroller_log.h"
#include "gamecontroller_errors.h"

namespace OHOS {
namespace GameController {
namespace {
constexpr char* COMMON_EVENT_DEVICE_STATUS_CHANGE = "usual.event.ohos.gamecontroller.device.status";
constexpr char* COMMON_EVENT_GAME_KEY_MAPPING_CHANGE = "usual.event.ohos.gamecontroller.game.keymapping.change";
constexpr char* COMMON_EVENT_OPEN_TEMPLATE_CONFIG = "usual.event.ohos.gamecontroller.open.template.config";
constexpr char* COMMON_EVENT_GAME_KEY_MAPPING_ENABLE = "usual.event.ohos.gamecontroller.game.keymapping.enable";
constexpr char* EVENT_PARAM_BUNDLE_NAME = "bundleName";
constexpr char* EVENT_PARAM_BUNDLE_VERSION = "version";
constexpr char* EVENT_PARAM_UNIQ = "uniq";
constexpr char* EVENT_PARAM_VID_PID = "vidPid";
constexpr char* EVENT_PARAM_DEVICE_TYPE = "deviceType";
constexpr char* EVENT_PARAM_DEVICE_STATUS = "deviceStatus";
constexpr char* EVENT_PARAM_DEVICE_NAME = "deviceName";
constexpr char* EVENT_PARAM_GAME_PID = "gamePid";

const int32_t ALL_SUBSCRIBER_TYPE = 0;
const int32_t SYSTEM_SUBSCRIBER_TYPE = 1;
}

EventPublisher::EventPublisher()
{
}

EventPublisher::~EventPublisher()
{
}

void EventPublisher::SendGameKeyMappingConfigChangeNotify(const GameKeyMappingInfo &gameKeyMappingInfo)
{
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    publishInfo.SetBundleName(gameKeyMappingInfo.bundleName);
    publishInfo.SetSubscriberType(ALL_SUBSCRIBER_TYPE);

    AAFwk::Want want;
    want.SetAction(COMMON_EVENT_GAME_KEY_MAPPING_CHANGE);
    want.SetBundle(gameKeyMappingInfo.bundleName);
    want.SetParam(EVENT_PARAM_BUNDLE_NAME, gameKeyMappingInfo.bundleName);
    want.SetParam(EVENT_PARAM_UNIQ, gameKeyMappingInfo.uniq);
    want.SetParam(EVENT_PARAM_DEVICE_TYPE, static_cast<int32_t>(gameKeyMappingInfo.deviceType));

    EventFwk::CommonEventData event;
    event.SetWant(want);
    HILOGI("[PUB]GameKeyMappingConfigChangeNotify, bundle[%{public}s].",
           gameKeyMappingInfo.bundleName.c_str());
    if (EventFwk::CommonEventManager::NewPublishCommonEvent(event, publishInfo) != 0) {
        HILOGE("[PUB]GameKeyMappingConfigChangeNotify error, bundle[%{public}s].",
               gameKeyMappingInfo.bundleName.c_str());
    }
}

int32_t EventPublisher::SendDeviceInfoNotify(const GameInfo &gameInfo,
                                             const DeviceInfo &deviceInfo,
                                             const int32_t &gamePid)
{
    if (!gameInfo.CheckParamValid() || !deviceInfo.CheckParamValid()) {
        return GAME_ERR_ARGUMENT_INVALID;
    }
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    publishInfo.SetSubscriberType(SYSTEM_SUBSCRIBER_TYPE);
    EventFwk::CommonEventData event = BuildCommonEventData(COMMON_EVENT_DEVICE_STATUS_CHANGE, gameInfo, deviceInfo,
                                                           gamePid);
    HILOGI("[PUB]DeviceInfoNotify, bundle[%{public}s], deviceName[%{public}s].",
           gameInfo.bundleName.c_str(), deviceInfo.name.c_str());
    if (EventFwk::CommonEventManager::NewPublishCommonEvent(event, publishInfo) != 0) {
        HILOGE("[PUB]DeviceInfoNotify error, bundle[%{public}s].",
               gameInfo.bundleName.c_str());
    }
    return GAME_CONTROLLER_SUCCESS;
}

int32_t EventPublisher::SendOpenTemplateConfigNotify(const GameInfo &gameInfo, const DeviceInfo &deviceInfo)
{
    if (!gameInfo.CheckParamValid() || !deviceInfo.CheckParamValid()) {
        return GAME_ERR_ARGUMENT_INVALID;
    }
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    publishInfo.SetSubscriberType(SYSTEM_SUBSCRIBER_TYPE);

    EventFwk::CommonEventData event = BuildCommonEventData(COMMON_EVENT_OPEN_TEMPLATE_CONFIG, gameInfo, deviceInfo, 0);
    HILOGI("[PUB]OpenTemplateConfigNotify, bundle[%{public}s], deviceName[%{public}s].",
           gameInfo.bundleName.c_str(), deviceInfo.name.c_str());
    if (EventFwk::CommonEventManager::NewPublishCommonEvent(event, publishInfo) != 0) {
        HILOGE("[PUB]OpenTemplateConfigNotify error, bundle[%{public}s].",
               gameInfo.bundleName.c_str());
    }
    return GAME_CONTROLLER_SUCCESS;
}

EventFwk::CommonEventData EventPublisher::BuildCommonEventData(const std::string &action,
                                                               const GameInfo &gameInfo,
                                                               const DeviceInfo &deviceInfo,
                                                               const int32_t &gamePid)
{
    AAFwk::Want want;
    want.SetAction(action);
    want.SetBundle(gameInfo.bundleName);
    want.SetParam(EVENT_PARAM_BUNDLE_NAME, gameInfo.bundleName);
    want.SetParam(EVENT_PARAM_BUNDLE_VERSION, gameInfo.version);
    want.SetParam(EVENT_PARAM_DEVICE_NAME, deviceInfo.name);
    want.SetParam(EVENT_PARAM_DEVICE_TYPE, static_cast<int32_t>(deviceInfo.deviceType));
    want.SetParam(EVENT_PARAM_DEVICE_STATUS, deviceInfo.status);
    want.SetParam(EVENT_PARAM_UNIQ, deviceInfo.uniq);
    want.SetParam(EVENT_PARAM_VID_PID, deviceInfo.GetVidPid());
    if (gamePid != 0) {
        want.SetParam(EVENT_PARAM_GAME_PID, gamePid);
    }
    EventFwk::CommonEventData event;
    event.SetWant(want);
    return event;
}

int32_t EventPublisher::SendEnableGameKeyMappingNotify(const GameInfo &gameInfo, bool isEnable)
{
    if (!gameInfo.CheckParamValid()) {
        return GAME_ERR_ARGUMENT_INVALID;
    }
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    publishInfo.SetBundleName(gameInfo.bundleName);
    publishInfo.SetSubscriberType(ALL_SUBSCRIBER_TYPE);

    AAFwk::Want want;
    want.SetAction(COMMON_EVENT_GAME_KEY_MAPPING_ENABLE);
    want.SetBundle(gameInfo.bundleName);
    want.SetParam(EVENT_PARAM_BUNDLE_NAME, gameInfo.bundleName);
    EventFwk::CommonEventData event;
    event.SetWant(want);
    HILOGI("[PUB]SendEnableGameKeyMappingNotify, bundle[%{public}s], isEnable[%{public}d].",
           gameInfo.bundleName.c_str(), isEnable ? 1 : 0);
    if (EventFwk::CommonEventManager::NewPublishCommonEvent(event, publishInfo) != 0) {
        HILOGE("[PUB]SendEnableGameKeyMappingNotify error, bundle[%{public}s].",
               gameInfo.bundleName.c_str());
    }
    return GAME_CONTROLLER_SUCCESS;
}
}
}