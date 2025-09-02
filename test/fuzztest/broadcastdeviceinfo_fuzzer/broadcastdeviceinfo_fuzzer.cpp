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
#include "broadcastdeviceinfo_fuzzer.h"
#include "gamecontroller_server_ability.h"
#include "gamecontroller_constants.h"
#include "igame_controller_server_interface.h"
#include "../mock/sa_mock.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t DEVICE_TYPE_NUM = 4;
}
void TestBroadcastDeviceInfo(sptr<GameControllerServerAbility> service,
                             const GetGameKeyMappingInfoParam &param)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInterfaceToken(u"OHOS.GameController.IGameControllerServerInterface");
    if (!data.WriteParcelable(&param)) {
        return;
    }

    service->OnRemoteRequest(
        static_cast<uint32_t>(IGameControllerServerInterfaceIpcCode::COMMAND_GET_GAME_KEY_MAPPING_CONFIG),
        data, reply, option);
}

void BroadcastDeviceInfo(const uint8_t* rawData, size_t size)
{
    GetGameKeyMappingInfoParam param;
    param.bundleName = std::string(reinterpret_cast<const char*>(rawData), size)
        .substr(0, MAX_BUNDLE_NAME_LENGTH);
    param.deviceType = static_cast<DeviceTypeEnum>(static_cast<int32_t>(*rawData) % DEVICE_TYPE_NUM);
    auto service_ = sptr<GameControllerServerAbilityEx>::MakeSptr(GAME_CONTROLLER_SA_ID);
    TestBroadcastDeviceInfo(service_, param);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GameController::BroadcastDeviceInfo(data, size);
    return 0;
}