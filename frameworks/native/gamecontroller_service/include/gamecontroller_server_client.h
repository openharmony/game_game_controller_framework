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

#ifndef GAME_CONTROLLER_GAMECONTROLLER_SERVER_CLIENT_H
#define GAME_CONTROLLER_GAMECONTROLLER_SERVER_CLIENT_H

#include <singleton.h>
#include "gamecontroller_client_model.h"
#include "gamecontroller_keymapping_model.h"

namespace OHOS {
namespace GameController {
class GameControllerServerClient : public DelayedSingleton<GameControllerServerClient> {
DECLARE_DELAYED_SINGLETON(GameControllerServerClient);
public:
    /**
     * Perform device identification.
     * @param deviceInfos Information about the device to be identified
     * @param identifyResult Device identification result
     * @return Interface Invoking Result
     */
    virtual int32_t IdentifyDevice(const std::vector<DeviceInfo> &deviceInfos,
                                   std::vector<DeviceInfo> &identifyResult);

    /**
     * Synchronize Identified device information.
     * @param deviceInfos Identified device information.
     * @return Interface Invoking Result
     */
    virtual int32_t SyncIdentifiedDeviceInfos(const std::vector<IdentifiedDeviceInfo> &deviceInfos);

    /**
     * Determines whether the game supports key mapping.
     * @param gameInfo Game Information
     * @param resultGameInfo Check Result
     * @return Interface Invoking Result
     */
    virtual int32_t IsSupportGameKeyMapping(const GameInfo &gameInfo, GameInfo &resultGameInfo);

    /**
     * Synchronize games that support key mapping (It can be invoked only by system service.)
     * @param If the value is true, full synchronization is performed. If the value is false,
     * only one record is updated.
     * @param gameInfos Synchronized game information
     * @return Interface Invoking Result
     */
    virtual int32_t SyncSupportKeyMappingGames(bool isSyncAll, const std::vector<GameInfo> &gameInfos);

    /**
    * Getting the Game Key Mapping Configuration
    * @param param the request param
    * @param gameKeyMappingInfo Game Key Mapping Configuration
    * @return Interface Invoking Result
    */
    virtual int32_t GetGameKeyMappingConfig(const GetGameKeyMappingInfoParam &param,
                                            GameKeyMappingInfo &gameKeyMappingInfo);

    /**
     * Set custom game key mapping configuration (It can be invoked only by system service.)
     * @param gameKeyMappingInfo the custom game key mapping configuration
     * @return Interface Invoking Result
     */
    virtual int32_t SetCustomGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfo);

    /**
     * Set default game key mapping configuration (It can be invoked only by system service.)
     * @param gameKeyMappingInfo default game key mapping configuration
     * @return Interface Invoking Result
     */
    virtual int32_t SetDefaultGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfo);

    /**
     * broadcast the device information
     * @param gameInfo game information
     * @param deviceInfo device information
     * @return Interface Invoking Result
     */
    virtual int32_t BroadcastDeviceInfo(const GameInfo &gameInfo, const DeviceInfo &deviceInfo);

    /**
     * Broadcast open the template configuration page
     * @param gameInfo game information
     * @param deviceInfo  device information
     * @return Interface Invoking Result
     */
    virtual int32_t BroadcastOpenTemplateConfig(const GameInfo &gameInfo, const DeviceInfo &deviceInfo);
};
}
}
#endif //GAME_CONTROLLER_GAMECONTROLLER_SERVER_CLIENT_H
