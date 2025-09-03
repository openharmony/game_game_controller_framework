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
#include "syncidentifieddeviceinfos_fuzzer.h"
#include "gamecontroller_server_ability.h"
#include "gamecontroller_constants.h"
#include "igame_controller_server_interface.h"
#include "../mock/sa_mock.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t MAX_DEVICE_NUMBER = 10000;
const int32_t MAX_DEVICES = 100;
const int32_t DEVICE_TYPE_NUM = 4;
}

void TestSyncIdentifiedDeviceInfos(sptr<GameControllerServerAbility> service,
                                   const std::vector<IdentifiedDeviceInfo> &deviceInfos)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInterfaceToken(u"OHOS.GameController.IGameControllerServerInterface");
    data.WriteInt32(deviceInfos.size());
    for (auto it3 = deviceInfos.begin(); it3 != deviceInfos.end(); ++it3) {
        if (!data.WriteParcelable(&(*it3))) {
            return;
        }
    }
    service->OnRemoteRequest(
        static_cast<uint32_t>(IGameControllerServerInterfaceIpcCode::COMMAND_SYNC_IDENTIFIED_DEVICE_INFOS),
        data, reply, option);
}

void TestIdentifyDevice(sptr<GameControllerServerAbility> service, std::vector<DeviceInfo> deviceInfos)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInterfaceToken(u"OHOS.GameController.IGameControllerServerInterface");
    data.WriteInt32(deviceInfos.size());
    for (auto it1 = deviceInfos.begin(); it1 != deviceInfos.end(); ++it1) {
        data.WriteParcelable(&(*it1));
    }
    service->OnRemoteRequest(static_cast<uint32_t>(IGameControllerServerInterfaceIpcCode::COMMAND_IDENTIFY_DEVICE),
                             data, reply, option);
}

void SyncIdentifiedDeviceInfos(const uint8_t* rawData, size_t size)
{
    auto count = static_cast<int32_t>(*rawData);
    if (count > MAX_DEVICE_NUMBER || count <= 0) {
        count = MAX_DEVICE_NUMBER;
    }
    std::vector<IdentifiedDeviceInfo> identifiedDeviceInfos;
    std::vector<DeviceInfo> deviceInfos;
    for (int32_t index = 0; index < count; index++) {
        IdentifiedDeviceInfo identifiedDeviceInfo;
        identifiedDeviceInfo.name = std::string(reinterpret_cast<const char*>(rawData), size)
            .substr(0, MAX_DEVICE_NAME_LENGTH);
        identifiedDeviceInfo.vendor = static_cast<int32_t >(*rawData);
        identifiedDeviceInfo.product = static_cast<int32_t >(*rawData);
        int32_t deviceType = static_cast<int32_t >(*rawData);
        identifiedDeviceInfo.deviceType = static_cast<DeviceTypeEnum>(deviceType % DEVICE_TYPE_NUM);
        identifiedDeviceInfos.push_back(identifiedDeviceInfo);

        if (index <= MAX_DEVICES) {
            DeviceInfo deviceInfo;
            deviceInfo.name = identifiedDeviceInfo.name;
            deviceInfo.uniq = std::string(reinterpret_cast<const char*>(rawData), size).substr(0, MAX_UNIQ_LENGTH);
            deviceInfo.vendor = identifiedDeviceInfo.vendor;
            deviceInfo.product = identifiedDeviceInfo.product;
            deviceInfos.push_back(deviceInfo);
        }
    }

    auto service_ = sptr<GameControllerServerAbilityEx>::MakeSptr(GAME_CONTROLLER_SA_ID);
    TestSyncIdentifiedDeviceInfos(service_, identifiedDeviceInfos);
    TestIdentifyDevice(service_, deviceInfos);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GameController::SyncIdentifiedDeviceInfos(data, size);
    return 0;
}