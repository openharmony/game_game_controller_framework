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

#ifndef GAME_CONTROLLER_EVENT_PUBLISHER_H
#define GAME_CONTROLLER_EVENT_PUBLISHER_H

#include <memory>
#include <singleton.h>
#include <gamecontroller_keymapping_model.h>
#include <common_event_manager.h>

namespace OHOS {
namespace GameController {
class EventPublisher : public DelayedSingleton<EventPublisher> {
DECLARE_DELAYED_SINGLETON(EventPublisher)

public:
    /**
     * Notify the GameKeyMappingConfigChange when change template
     * @param gameKeyMappingInfo GameKeyMappingInfo
     */
    void SendGameKeyMappingConfigChangeNotify(const GameKeyMappingInfo &gameKeyMappingInfo);

    /**
     * Notify deviceInfo when device status changed
     * @param gameInfo gameInfo
     * @param deviceInfo deviceInfo
     * @param gamePid game's pid
     * @return result
     */
    int32_t SendDeviceInfoNotify(const GameInfo &gameInfo, const DeviceInfo &deviceInfo, const int32_t &gamePid);

    /**
     * Notify open template configuration page
     * @param gameInfo gameInfo
     * @param deviceInfo deviceInfo
     * @return result
     */
    int32_t SendOpenTemplateConfigNotify(const GameInfo &gameInfo, const DeviceInfo &deviceInfo);

    /**
     * Notify input-to-touch for games with supported key mapping.
     * @param gameInfo gameInfo
     * @param  isEnable true means enable
     * @return result
     */
    int32_t SendEnableGameKeyMappingNotify(const GameInfo &gameInfo, bool isEnable);

    /**
     * Send a notification of changes to the list of games that support keymapping
     */
    void SendSupportedKeyMappingChangeNotify();

private:
    EventFwk::CommonEventData BuildCommonEventData(const std::string &action,
                                                   const GameInfo &gameInfo, const DeviceInfo &deviceInfo,
                                                   const int32_t &gamePid);
};
}
}
#endif //GAME_CONTROLLER_EVENT_PUBLISHER_H
