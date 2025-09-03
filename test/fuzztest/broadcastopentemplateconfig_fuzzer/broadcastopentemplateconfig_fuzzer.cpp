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
#include "broadcastopentemplateconfig_fuzzer.h"
#include "gamecontroller_server_ability.h"
#include "gamecontroller_constants.h"
#include "igame_controller_server_interface.h"
#include "../mock/sa_mock.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t DEVICE_TYPE_NUM = 4;
}
void TestBroadcastOpenTemplateConfig(sptr<GameControllerServerAbility> service,
                                     const GameInfo &gameInfo,
                                     const DeviceInfo &deviceInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInterfaceToken(u"OHOS.GameController.IGameControllerServerInterface");
    if (!data.WriteParcelable(&gameInfo)) {
        return;
    }
    if (!data.WriteParcelable(&deviceInfo)) {
        return;
    }

    service->OnRemoteRequest(
        static_cast<uint32_t>(IGameControllerServerInterfaceIpcCode::COMMAND_BROADCAST_OPEN_TEMPLATE_CONFIG),
        data, reply, option);
}

void BroadcastOpenTemplateConfig(const uint8_t* rawData, size_t size)
{
    GameInfo gameInfo;
    gameInfo.bundleName = std::string(reinterpret_cast<const char*>(rawData), size)
        .substr(0, MAX_BUNDLE_NAME_LENGTH);
    gameInfo.version = std::string(reinterpret_cast<const char*>(rawData), size)
        .substr(0, MAX_VERSION_LENGTH);
    DeviceInfo deviceInfo;
    deviceInfo.name = std::string(reinterpret_cast<const char*>(rawData), size)
        .substr(0, MAX_BUNDLE_NAME_LENGTH);
    deviceInfo.uniq = std::string(reinterpret_cast<const char*>(rawData), size)
        .substr(0, MAX_UNIQ_LENGTH);
    deviceInfo.product = static_cast<int32_t>(*rawData);
    deviceInfo.vendor = static_cast<int32_t>(*rawData);
    deviceInfo.status = static_cast<int32_t>(*rawData);
    deviceInfo.deviceType = static_cast<DeviceTypeEnum>(static_cast<int32_t>(*rawData) % DEVICE_TYPE_NUM);
    auto service_ = sptr<GameControllerServerAbilityEx>::MakeSptr(GAME_CONTROLLER_SA_ID);
    TestBroadcastOpenTemplateConfig(service_, gameInfo, deviceInfo);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GameController::BroadcastOpenTemplateConfig(data, size);
    return 0;
}