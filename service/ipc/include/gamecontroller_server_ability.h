/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GAME_CONTROLLER_GAMECONTROLLER_SERVER_ABILITY_H
#define GAME_CONTROLLER_GAMECONTROLLER_SERVER_ABILITY_H

#include "system_ability.h"
#include "refbase.h"
#include "game_controller_server_interface_stub.h"

namespace OHOS {
namespace GameController {

class GameControllerServerAbility : public SystemAbility, public GameControllerServerInterfaceStub {
DECLARE_SYSTEM_ABILITY(GameControllerServerAbility)

public:
    explicit GameControllerServerAbility(int32_t systemAbilityId, bool runOnCreate = false);

    ~GameControllerServerAbility() override;

public:
    /**
    * Perform device identification.
    * @param deviceInfos Information about the device to be identified
    * @param identifyResult Identification result
    * @return 0: success. Other values fail.
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

    /**
     * Enable input-to-touch for games with supported key mapping.
     * @param gameInfo game information
     * @param isEnable true means enable
     * @return Interface Invoking Result
     */
    virtual int32_t EnableGameKeyMapping(const GameInfo &gameInfo, bool isEnable);

protected:
    /**
     * SA Startup Event
     * @param startReason Startup Reason
     */
    void OnStart(const SystemAbilityOnDemandReason &startReason) override;

    /**
     * SA Stop Event
     * @param stopReason Stop Reason
     */
    void OnStop(const SystemAbilityOnDemandReason &stopReason) override;

    /**
     * SA Idle Event
     * @param idleReason Idle Reason
     * @return The value 0 indicates that the user can be idle.
     */
    int32_t OnIdle(const SystemAbilityOnDemandReason &idleReason) override;

    /**
     * SA activation event
     * @param activeReason Activation Reason
     */
    void OnActive(const SystemAbilityOnDemandReason &activeReason) override;

    /**
     * Indicates whether the call comes from a system service
     * @return true: it's system service call
     */
    virtual bool IsSystemServiceCall();

    /**
     * Indicates whether the call comes from a system app.
     * @return true: it's system app call
     */
    virtual bool IsSystemAppCall();

    /**
     * Check whether bundleName is the same as bundleName of the current invoker.
     * @param bundleName bundleName
     * @return true: Yes
     */
    virtual bool VerifyBundleNameIsValid(const std::string &bundleName);

    virtual bool IsSupportGameKeyMapping(const GameInfo &gameInfo);

private:
    bool ready_ = false;
};
}
}
#endif //GAME_CONTROLLER_GAMECONTROLLER_SERVER_ABILITY_H
