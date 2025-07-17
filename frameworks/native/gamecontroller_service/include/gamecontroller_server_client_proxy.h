/*
 *  Copyright (c) 2025 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef GAME_CONTROLLER_GAMECONTROLLER_CLIENT_PROXY_H
#define GAME_CONTROLLER_GAMECONTROLLER_CLIENT_PROXY_H

#include <singleton.h>
#include <system_ability_load_callback_stub.h>
#include <iremote_object.h>
#include <condition_variable>
#include "igame_controller_server_interface.h"
#include "gamecontroller_client_model.h"
#include "gamecontroller_keymapping_model.h"

namespace OHOS {
namespace GameController {
/**
 * GameControllerSA loading callback result
 */
class GameControllerSaLoadCallback : public SystemAbilityLoadCallbackStub {
public:
    void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject);

    void OnLoadSystemAbilityFail(int32_t systemAbilityId);
};

/**
 * GameControllerSA Death Listening
 */
class GameControllerSaDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit GameControllerSaDeathRecipient();

    virtual ~GameControllerSaDeathRecipient() override = default;

    void OnRemoteDied(const wptr<IRemoteObject> &object) override;
};

class GameControllerServerClientProxy : public DelayedSingleton<GameControllerServerClientProxy> {
DECLARE_DELAYED_SINGLETON(GameControllerServerClientProxy);
public:
    /**
     * The SA is loaded successfully.
     * @param remoteObject IRemoteObject
     */
    void LoadSystemAbilitySuccess(const sptr<IRemoteObject> &remoteObject);

    /**
     * Failed to load the SA.
     */
    void LoadSystemAbilityFail();

    /**
     * SA Death Treatment
     * @param object IRemoteObject
     */
    void OnRemoteSaDead(const wptr<IRemoteObject> &object);

    /**
     * Perform device identification.
     * @param deviceInfos Information about the device to be identified
     * @param identifyResult Device identification result
     * @return Interface Invoking Result
     */
    int32_t IdentifyDevice(const std::vector<DeviceInfo> &deviceInfos, std::vector<DeviceInfo> &identifyResult);

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
    int32_t IsSupportGameKeyMapping(const GameInfo &gameInfo, GameInfo &resultGameInfo);

    /**
     * Synchronize games that support key mapping
     * @param If the value is true, full synchronization is performed. If the value is false,
     * only one record is updated.
     * @param gameInfos Synchronized game information
     * @return
     */
    int32_t SyncSupportKeyMappingGames(bool isSyncAll, const std::vector<GameInfo> &resultGameInfo);

    /**
    * Getting the Game Key Mapping Configuration
    * @param param the request param
    * @param gameKeyMappingInfo Game Key Mapping Configuration
    * @return Interface Invoking Result
    */
    int32_t GetGameKeyMappingConfig(const GetGameKeyMappingInfoParam &param,
                                    GameKeyMappingInfo &gameKeyMappingInfo);

    /**
     * Set custom game key mapping configuration
     * @param gameKeyMappingInfo the custom game key mapping configuration
     * @return Interface Invoking Result
     */
    int32_t SetCustomGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfo);

    /**
     * Set default game key mapping configuration
     * @param gameKeyMappingInfo default game key mapping configuration
     * @return Interface Invoking Result
     */
    int32_t SetDefaultGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfo);

    /**
     * broadcast the device information
     * @param gameInfo game information
     * @param deviceInfo device information
     * @return Interface Invoking Result
     */
    int32_t BroadcastDeviceInfo(const GameInfo &gameInfo, const DeviceInfo &deviceInfo);

    /**
     * Broadcast open the template configuration page
     * @param gameInfo game information
     * @param deviceInfo  device information
     * @return Interface Invoking Result
     */
    int32_t BroadcastOpenTemplateConfig(const GameInfo &gameInfo, const DeviceInfo &deviceInfo);

private:
    sptr<IGameControllerServerInterface> GetServiceProxy();

    void ReleaseServiceProxy();

private:
    std::mutex loadServiceLock_;
    std::condition_variable loadServiceConVar_;
    sptr<IGameControllerServerInterface> gamecontrollerServerProxy_ = nullptr;
    sptr<GameControllerSaDeathRecipient> deathRecipient_;
};
}
}

#endif //GAME_CONTROLLER_GAMECONTROLLER_CLIENT_PROXY_H
