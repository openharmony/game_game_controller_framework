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

#include <ipc_skeleton.h>
#include <accesstoken_kit.h>
#include <bundle_mgr_client.h>
#include "gamecontroller_server_ability.h"
#include "gamecontroller_log.h"
#include "gamecontroller_constants.h"
#include "gamecontroller_errors.h"
#include "device_manager.h"
#include "ability_event_handler.h"
#include "key_mapping_config_manager.h"
#include "game_support_key_mapping_manager.h"
#include "event_publisher.h"
#include "permission_utils.h"

using namespace OHOS::AppExecFwk;
using namespace OHOS::Security::AccessToken;
namespace OHOS {
namespace GameController {

REGISTER_SYSTEM_ABILITY_BY_ID(GameControllerServerAbility, GAME_CONTROLLER_SA_ID, true);

GameControllerServerAbility::GameControllerServerAbility(int32_t saId, bool runOnCreate) : SystemAbility(saId,
                                                                                                         runOnCreate)
{
    HILOGI("GameControllerServerAbility()");
}

GameControllerServerAbility::~GameControllerServerAbility()
{
    HILOGI("GameControllerServerAbility()");
}

void GameControllerServerAbility::OnStart(const SystemAbilityOnDemandReason &startReason)
{
    HILOGI("Begin to start, id[%{public}d], name[%{public}s], value[%{public}s].",
           static_cast<int32_t>(startReason.GetId()), startReason.GetName().c_str(), startReason.GetValue().c_str());
    if (ready_) {
        HILOGW("Start more than one time.");
        return;
    }
    if (!Publish(this)) {
        HILOGE("Fail to register to system ability manager.");
        return;
    }
    ready_ = true;
    HILOGI("End start successfully.");

    /**
     * To ensure that the sa is normally uninstalled,
     * the sa is automatically uninstalled every 30 seconds after the sa is loaded.
     */
    DelayedSingleton<AbilityEventHandler>::GetInstance()->DelayUnloadService();
}

void GameControllerServerAbility::OnStop(const SystemAbilityOnDemandReason &stopReason)
{
    HILOGI("Begin to stop, id[%{public}d], name[%{public}s], value[%{public}s].",
           static_cast<int32_t>(stopReason.GetId()), stopReason.GetName().c_str(), stopReason.GetValue().c_str());
}

int32_t GameControllerServerAbility::OnIdle(const SystemAbilityOnDemandReason &idleReason)
{
    HILOGI("Begin to idle, id[%{public}d], name[%{public}s], value[%{public}s].",
           static_cast<int32_t>(idleReason.GetId()), idleReason.GetName().c_str(), idleReason.GetValue().c_str());
    return GAME_CONTROLLER_SUCCESS;
}

void GameControllerServerAbility::OnActive(const SystemAbilityOnDemandReason &activeReason)
{
    HILOGI("Begin to active, id[%{public}d], name[%{public}s], value[%{public}s].",
           static_cast<int32_t>(activeReason.GetId()), activeReason.GetName().c_str(), activeReason.GetValue().c_str());
    DelayedSingleton<AbilityEventHandler>::GetInstance()->DelayUnloadService();
}

int32_t GameControllerServerAbility::IdentifyDevice(const std::vector<DeviceInfo> &deviceInfos,
                                                    std::vector<DeviceInfo> &identifyResult)
{
    return DelayedSingleton<DeviceManager>::GetInstance()->DeviceIdentify(deviceInfos, identifyResult);
}

int32_t GameControllerServerAbility::SetCustomGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfo)
{
    if (!IsSystemServiceCall()) {
        HILOGE("no sys permission");
        return GAME_ERR_NO_SYS_PERMISSIONS;
    }

    return DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(gameKeyMappingInfo);
}

int32_t GameControllerServerAbility::GetGameKeyMappingConfig(const GetGameKeyMappingInfoParam &param,
                                                             GameKeyMappingInfo &gameKeyMappingInfo)
{
    return DelayedSingleton<KeyMappingConfigManager>::GetInstance()->GetGameKeyMappingConfig(param,
                                                                                             gameKeyMappingInfo);
}

int32_t GameControllerServerAbility::SyncIdentifiedDeviceInfos(const std::vector<IdentifiedDeviceInfo> &deviceInfos)
{
    if (!IsSystemServiceCall()) {
        HILOGE("no sys permission");
        return GAME_ERR_NO_SYS_PERMISSIONS;
    }
    return DelayedSingleton<DeviceManager>::GetInstance()->SyncIdentifiedDeviceInfos(deviceInfos);
}

int32_t GameControllerServerAbility::SyncSupportKeyMappingGames(bool isSyncAll, const std::vector<GameInfo> &gameInfos)
{
    if (!IsSystemServiceCall()) {
        HILOGE("no sys permission");
        return GAME_ERR_NO_SYS_PERMISSIONS;
    }
    return DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->SyncSupportKeyMappingGames(isSyncAll,
                                                                                                     gameInfos);
}

int32_t GameControllerServerAbility::SetDefaultGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfo)
{
    if (!IsSystemServiceCall()) {
        HILOGE("no sys permission");
        return GAME_ERR_NO_SYS_PERMISSIONS;
    }
    return DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(gameKeyMappingInfo);
}

int32_t GameControllerServerAbility::BroadcastDeviceInfo(const GameInfo &gameInfo, const DeviceInfo &deviceInfo)
{
    // 1.check the gameInfo.bundleName is same with the caller.
    if (!VerifyBundleNameIsValid(gameInfo.bundleName)) {
        HILOGE("no sys permission");
        return GAME_ERR_NO_SYS_PERMISSIONS;
    }
    
    // 2. check the bundleName is in keyMapping trustlist.
    if (IsSupportGameKeyMapping(gameInfo)) {
        int32_t gamePid = IPCSkeleton::GetCallingPid();
        HILOGI("gamePid [%{public}d]", gamePid);
        return DelayedSingleton<EventPublisher>::GetInstance()->SendDeviceInfoNotify(gameInfo, deviceInfo, gamePid);
    }
    return GAME_ERR_NO_SYS_PERMISSIONS;
}

int32_t GameControllerServerAbility::BroadcastOpenTemplateConfig(const GameInfo &gameInfo, const DeviceInfo &deviceInfo)
{
    // 1. check the gameInfo.bundleName is same with the caller.
    if (!VerifyBundleNameIsValid(gameInfo.bundleName)) {
        HILOGE("no sys permission");
        return GAME_ERR_NO_SYS_PERMISSIONS;
    }

    // 2. check the bundleName is in keyMapping trustlist.
    if (IsSupportGameKeyMapping(gameInfo)) {
        return DelayedSingleton<EventPublisher>::GetInstance()->SendOpenTemplateConfigNotify(gameInfo, deviceInfo);
    }
    return GAME_ERR_NO_SYS_PERMISSIONS;
}

bool GameControllerServerAbility::IsSystemServiceCall()
{
    return DelayedSingleton<PermissionUtils>::GetInstance()->IsSACall();
}

bool GameControllerServerAbility::VerifyBundleNameIsValid(const std::string &bundleName)
{
    int32_t uid = GetCallingUid();
    return DelayedSingleton<PermissionUtils>::GetInstance()->VerifyBundleNameIsValid(bundleName, uid);
}

bool GameControllerServerAbility::IsSupportGameKeyMapping(const GameInfo &gameInfo)
{
    GameInfo resultGameInfo;
    DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->IsSupportGameKeyMapping(gameInfo, resultGameInfo);
    if (resultGameInfo.isSupportKeyMapping) {
        return true;
    }
    return false;
}

int32_t GameControllerServerAbility::EnableGameKeyMapping(const GameInfo &gameInfo, bool isEnable)
{
    if (!IsSystemServiceCall()) {
        HILOGE("no sys permission");
        return GAME_ERR_NO_SYS_PERMISSIONS;
    }
    return DelayedSingleton<EventPublisher>::GetInstance()->SendEnableGameKeyMappingNotify(gameInfo, isEnable);
}
}
}