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
#include <cstdint>
#include "syncsupportkeymappinggames_fuzzer.h"
#include "gamecontroller_constants.h"
#include "igame_controller_server_interface.h"
#include "gamecontroller_log.h"
#include "gamecontroller_server_ability.h"
#include "../mock/sa_mock.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t MAX_CONFIG_NUM = 200;
}

void TestSyncSupportKeyMappingGames(sptr<GameControllerServerAbility> service, bool isSyncAll,
                                    const std::vector<GameInfo> &gameInfos)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInterfaceToken(u"OHOS.GameController.IGameControllerServerInterface");
    data.WriteInt32(isSyncAll ? 1 : 0);
    data.WriteInt32(gameInfos.size());
    for (auto it1 = gameInfos.begin(); it1 != gameInfos.end(); ++it1) {
        data.WriteParcelable(&(*it1));
    }
    service->OnRemoteRequest(
        static_cast<uint32_t>(IGameControllerServerInterfaceIpcCode::COMMAND_SYNC_SUPPORT_KEY_MAPPING_GAMES),
        data, reply, option);
}

void SyncSupportKeyMappingGames(const uint8_t* rawData, size_t size)
{
    auto service_ = sptr<GameControllerServerAbilityEx>::MakeSptr(GAME_CONTROLLER_SA_ID);
    auto count = static_cast<int32_t>(*rawData);
    if (count > MAX_CONFIG_NUM || count <= 0) {
        count = MAX_CONFIG_NUM;
    }
    std::vector<GameInfo> gameInfos;
    for (int32_t index = 0; index < count; index++) {
        GameInfo gameInfo;
        gameInfo.bundleName = std::string(reinterpret_cast<const char*>(rawData), size)
            .substr(0, MAX_BUNDLE_NAME_LENGTH);
        gameInfo.version = std::string(reinterpret_cast<const char*>(rawData), size)
            .substr(0, MAX_VERSION_LENGTH);
        gameInfo.isSupportKeyMapping = static_cast<bool>(*rawData);
        gameInfos.push_back(gameInfo);
    }
    bool isSyncAll = static_cast<bool>(*rawData);
    TestSyncSupportKeyMappingGames(service_, isSyncAll, gameInfos);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GameController::SyncSupportKeyMappingGames(data, size);
    return 0;
}