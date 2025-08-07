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
#include <iservice_registry.h>
#include "gamecontroller_server_client_proxy.h"
#include "gamecontroller_constants.h"
#include "gamecontroller_log.h"
#include "gamecontroller_errors.h"

namespace OHOS {
namespace GameController {
namespace {
/**
 * SA loading timeout interval, in milliseconds.
 */
const int32_t LOAD_SA_TIMEOUT_MS = 1000;
}

void GameControllerSaLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
                                                              const sptr<IRemoteObject> &remoteObject)
{
    if (systemAbilityId != GAME_CONTROLLER_SA_ID) {
        HILOGI("system ability is not gamecontroller_server:%d", systemAbilityId);
        return;
    }

    DelayedSingleton<GameControllerServerClientProxy>::GetInstance()->LoadSystemAbilitySuccess(remoteObject);
    HILOGI("Load gamecontroller_server systemAbility successed!");
}

void GameControllerSaLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    if (systemAbilityId != GAME_CONTROLLER_SA_ID) {
        HILOGW("system ability is not gamecontroller_server:%d", systemAbilityId);
        return;
    }
    DelayedSingleton<GameControllerServerClientProxy>::GetInstance()->LoadSystemAbilityFail();
    HILOGI("Load system ability failed!");
}

GameControllerSaDeathRecipient::GameControllerSaDeathRecipient()
{
}

void GameControllerSaDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    HILOGI("Remote gamecontroller_server systemAbility dead.");
    DelayedSingleton<GameControllerServerClientProxy>::GetInstance()->OnRemoteSaDead(object);
}

GameControllerServerClientProxy::GameControllerServerClientProxy()
{
    deathRecipient_ = sptr<GameControllerSaDeathRecipient>::MakeSptr();
}

GameControllerServerClientProxy::~GameControllerServerClientProxy()
{
    std::unique_lock<std::mutex> lock(loadServiceLock_);
    ReleaseServiceProxy();
}

void GameControllerServerClientProxy::LoadSystemAbilitySuccess(const sptr<IRemoteObject> &remoteObject)
{
    if (remoteObject == nullptr) {
        HILOGI("Load systemAbility success, but remoteObject is null.");
        return;
    }
    std::lock_guard<std::mutex> lock(loadServiceLock_);
    if (deathRecipient_ != nullptr) {
        remoteObject->AddDeathRecipient(deathRecipient_);
    }
    gamecontrollerServerProxy_ = iface_cast<IGameControllerServerInterface>(remoteObject);
    loadServiceConVar_.notify_all();
}

void GameControllerServerClientProxy::LoadSystemAbilityFail()
{
    std::lock_guard<std::mutex> lock(loadServiceLock_);
    gamecontrollerServerProxy_ = nullptr;
    loadServiceConVar_.notify_all();
}

void GameControllerServerClientProxy::OnRemoteSaDead(const wptr<IRemoteObject> &object)
{
}

int32_t GameControllerServerClientProxy::IdentifyDevice(const std::vector<DeviceInfo> &deviceInfos,
                                                        std::vector<DeviceInfo> &identifyResult)
{
    sptr<IGameControllerServerInterface> serviceProxy = GetServiceProxy();
    if (serviceProxy == nullptr) {
        HILOGE("get GameControllerServerProxy failed!");
        return GAME_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t ret = serviceProxy->IdentifyDevice(deviceInfos, identifyResult);
    if (ret != GAME_CONTROLLER_SUCCESS) {
        HILOGE("serviceProxy IdentifyDevice ret=[%{public}d]", ret);
    }
    return ret;
}

sptr<IGameControllerServerInterface> GameControllerServerClientProxy::GetServiceProxy()
{
    std::unique_lock<std::mutex> lock(loadServiceLock_);

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        HILOGE("Get SystemAbilityManager failed.");
        return nullptr;
    }

    auto object = samgr->CheckSystemAbility(GAME_CONTROLLER_SA_ID);
    if (object == nullptr) {
        //If the SA does not exist, the current proxy object needs to be released and obtained again.
        HILOGW("Check systemAbility not exist and try to release game controller proxy.");
        ReleaseServiceProxy();
    } else {
        gamecontrollerServerProxy_ = iface_cast<IGameControllerServerInterface>(object);
        return gamecontrollerServerProxy_;
    }

    HILOGI("Try load systemAbility.");
    sptr<GameControllerSaLoadCallback> loadCallback = sptr<GameControllerSaLoadCallback>::MakeSptr();
    int32_t ret = samgr->LoadSystemAbility(GAME_CONTROLLER_SA_ID, loadCallback);
    if (ret != GAME_CONTROLLER_SUCCESS) {
        HILOGE("Failed to load systemAbility.");
        return gamecontrollerServerProxy_;
    }
    auto waitStatus = loadServiceConVar_.wait_for(lock, std::chrono::milliseconds(LOAD_SA_TIMEOUT_MS),
                                                  [this]() { return gamecontrollerServerProxy_ != nullptr; });
    if (!waitStatus) {
        HILOGE("Load systemAbility timeout.");
        return gamecontrollerServerProxy_;
    }
    HILOGI("Load systemAbility succeeded.");
    return gamecontrollerServerProxy_;
}

void GameControllerServerClientProxy::ReleaseServiceProxy()
{
    if (gamecontrollerServerProxy_ != nullptr) {
        auto remoteObject = gamecontrollerServerProxy_->AsObject();
        if (remoteObject != nullptr && deathRecipient_ != nullptr) {
            remoteObject->RemoveDeathRecipient(deathRecipient_);
        }
        gamecontrollerServerProxy_ = nullptr;
    }
}

int32_t GameControllerServerClientProxy::GetGameKeyMappingConfig(const GetGameKeyMappingInfoParam &param,
                                                                 GameKeyMappingInfo &gameKeyMappingInfo)
{
    sptr<IGameControllerServerInterface> serviceProxy = GetServiceProxy();
    if (serviceProxy == nullptr) {
        HILOGE("get GameControllerServerProxy failed!");
        return GAME_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t ret = serviceProxy->GetGameKeyMappingConfig(param, gameKeyMappingInfo);
    if (ret != GAME_CONTROLLER_SUCCESS) {
        HILOGE("GetGameKeyMappingConfig failed. ret=[%{public}d]", ret);
    }
    return ret;
}

int32_t GameControllerServerClientProxy::SetCustomGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfo)
{
    sptr<IGameControllerServerInterface> serviceProxy = GetServiceProxy();
    if (serviceProxy == nullptr) {
        HILOGE("get GameControllerServerProxy failed!");
        return GAME_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t ret = serviceProxy->SetCustomGameKeyMappingConfig(gameKeyMappingInfo);
    if (ret != GAME_CONTROLLER_SUCCESS) {
        HILOGE("SetCustomGameKeyMappingConfig failed. ret=[%{public}d]", ret);
    }
    return ret;
}

int32_t GameControllerServerClientProxy::BroadcastDeviceInfo(const GameInfo &gameInfo, const DeviceInfo &deviceInfo)
{
    sptr<IGameControllerServerInterface> serviceProxy = GetServiceProxy();
    if (serviceProxy == nullptr) {
        HILOGE("get GameControllerServerProxy failed!");
        return GAME_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t ret = serviceProxy->BroadcastDeviceInfo(gameInfo, deviceInfo);
    if (ret != GAME_CONTROLLER_SUCCESS) {
        HILOGE("BroadcastDeviceInfo failed. ret=[%{public}d]", ret);
    }
    return ret;
}

int32_t GameControllerServerClientProxy::SyncSupportKeyMappingGames(bool isSyncAll,
                                                                    const std::vector<GameInfo> &gameInfos)
{
    sptr<IGameControllerServerInterface> serviceProxy = GetServiceProxy();
    if (serviceProxy == nullptr) {
        HILOGE("get GameControllerServerProxy failed!");
        return GAME_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t ret = serviceProxy->SyncSupportKeyMappingGames(isSyncAll, gameInfos);
    if (ret != GAME_CONTROLLER_SUCCESS) {
        HILOGE("SyncSupportKeyMappingGames failed. ret=[%{public}d]", ret);
    }
    return ret;
}

int32_t GameControllerServerClientProxy::SetDefaultGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfo)
{
    sptr<IGameControllerServerInterface> serviceProxy = GetServiceProxy();
    if (serviceProxy == nullptr) {
        HILOGE("get GameControllerServerProxy failed!");
        return GAME_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t ret = serviceProxy->SetDefaultGameKeyMappingConfig(gameKeyMappingInfo);
    if (ret != GAME_CONTROLLER_SUCCESS) {
        HILOGE("SetDefaultGameKeyMappingConfig failed. ret=[%{public}d]", ret);
    }
    return ret;
}

int32_t GameControllerServerClientProxy::BroadcastOpenTemplateConfig(const GameInfo &gameInfo,
                                                                     const DeviceInfo &deviceInfo)
{
    sptr<IGameControllerServerInterface> serviceProxy = GetServiceProxy();
    if (serviceProxy == nullptr) {
        HILOGE("get GameControllerServerProxy failed!");
        return GAME_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t ret = serviceProxy->BroadcastOpenTemplateConfig(gameInfo, deviceInfo);
    if (ret != GAME_CONTROLLER_SUCCESS) {
        HILOGE("BroadcastOpenTemplateConfig failed. ret=[%{public}d]", ret);
    }
    return ret;
}

int32_t GameControllerServerClientProxy::SyncIdentifiedDeviceInfos(const std::vector<IdentifiedDeviceInfo> &deviceInfos)
{
    sptr<IGameControllerServerInterface> serviceProxy = GetServiceProxy();
    if (serviceProxy == nullptr) {
        HILOGE("get GameControllerServerProxy failed!");
        return GAME_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t ret = serviceProxy->SyncIdentifiedDeviceInfos(deviceInfos);
    if (ret != GAME_CONTROLLER_SUCCESS) {
        HILOGE("SyncIdentifiedDeviceInfos failed. ret=[%{public}d]", ret);
    }
    return ret;
}

int32_t GameControllerServerClientProxy::EnableGameKeyMapping(const GameInfo &gameInfo, const bool isEnable)
{
    sptr<IGameControllerServerInterface> serviceProxy = GetServiceProxy();
    if (serviceProxy == nullptr) {
        HILOGE("get GameControllerServerProxy failed!");
        return GAME_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t ret = serviceProxy->EnableGameKeyMapping(gameInfo, isEnable);
    if (ret != GAME_CONTROLLER_SUCCESS) {
        HILOGE("EnableGameKeyMapping failed. ret=[%{public}d]", ret);
    }
    return ret;
}

}
}


