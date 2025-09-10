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
#include "gamecontroller_constants.h"
#include "device_manager.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t MAX_DEVICE_NUMBER = 10000;
const int32_t MAX_DEVICES = 100;
const int32_t DEVICE_TYPE_NUM = 4;
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

    DelayedSingleton<DeviceManager>::GetInstance()->SyncIdentifiedDeviceInfos(identifiedDeviceInfos);

    std::vector<DeviceInfo> identifyResult;
    DelayedSingleton<DeviceManager>::GetInstance()->DeviceIdentify(deviceInfos, identifyResult);
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