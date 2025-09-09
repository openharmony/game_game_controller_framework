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
#include "gamecontroller_server_client.h"
#include "gamecontroller_server_client_proxy.h"

namespace OHOS {
namespace GameController {
GameControllerServerClient::GameControllerServerClient()
{
}

GameControllerServerClient::~GameControllerServerClient()
{
}

int32_t GameControllerServerClient::IdentifyDevice(const std::vector<DeviceInfo> &deviceInfos,
                                                   std::vector<DeviceInfo> &identifyResult)
{
    return DelayedSingleton<GameControllerServerClientProxy>::GetInstance()->IdentifyDevice(deviceInfos,
                                                                                            identifyResult);
}

int32_t GameControllerServerClient::GetGameKeyMappingConfig(const GetGameKeyMappingInfoParam &param,
                                                            GameKeyMappingInfo &gameKeyMappingInfo)
{
    return DelayedSingleton<GameControllerServerClientProxy>::GetInstance()
        ->GetGameKeyMappingConfig(param, gameKeyMappingInfo);
}

int32_t GameControllerServerClient::SetCustomGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfo)
{
    return DelayedSingleton<GameControllerServerClientProxy>::GetInstance()
        ->SetCustomGameKeyMappingConfig(gameKeyMappingInfo);
}

int32_t GameControllerServerClient::BroadcastDeviceInfo(const GameInfo &gameInfo, const DeviceInfo &deviceInfo)
{
    return DelayedSingleton<GameControllerServerClientProxy>::GetInstance()
        ->BroadcastDeviceInfo(gameInfo, deviceInfo);
}

int32_t GameControllerServerClient::SyncSupportKeyMappingGames(bool isSyncAll,
                                                               const std::vector<GameInfo> &gameInfos)
{
    return DelayedSingleton<GameControllerServerClientProxy>::GetInstance()
        ->SyncSupportKeyMappingGames(isSyncAll, gameInfos);
}

int32_t GameControllerServerClient::SetDefaultGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfo)
{
    return DelayedSingleton<GameControllerServerClientProxy>::GetInstance()
        ->SetDefaultGameKeyMappingConfig(gameKeyMappingInfo);
}

int32_t GameControllerServerClient::BroadcastOpenTemplateConfig(const GameInfo &gameInfo, const DeviceInfo &deviceInfo)
{
    return DelayedSingleton<GameControllerServerClientProxy>::GetInstance()
        ->BroadcastOpenTemplateConfig(gameInfo, deviceInfo);
}

int32_t GameControllerServerClient::SyncIdentifiedDeviceInfos(const std::vector<IdentifiedDeviceInfo> &deviceInfos)
{
    return DelayedSingleton<GameControllerServerClientProxy>::GetInstance()
        ->SyncIdentifiedDeviceInfos(deviceInfos);
}

int32_t GameControllerServerClient::EnableGameKeyMapping(const GameInfo &gameInfo, const bool isEnable)
{
    return DelayedSingleton<GameControllerServerClientProxy>::GetInstance()
        ->EnableGameKeyMapping(gameInfo, isEnable);
}

}
}