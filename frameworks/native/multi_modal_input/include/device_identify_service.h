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


#ifndef GAME_CONTROLLER_DEVICE_IDENTIFY_SERVICE_H
#define GAME_CONTROLLER_DEVICE_IDENTIFY_SERVICE_H

#include <singleton.h>
#include <unordered_map>
#include "gamecontroller_client_model.h"

namespace OHOS {
namespace GameController {
/**
 * Perform device identification.
 */
class DeviceIdentifyService : public DelayedSingleton<DeviceIdentifyService> {
DECLARE_DELAYED_SINGLETON(DeviceIdentifyService);
public:
    /**
     * Identifying Device Types in Batches
     * @param deviceInfos Devices to be identified
     * @return Identification result
     */
    virtual std::vector<DeviceInfo> IdentifyDeviceType(const std::vector<DeviceInfo> &deviceInfos);

private:
    bool HasFullKeyboard(const DeviceInfo &deviceInfo);
};
}
}

#endif //GAME_CONTROLLER_DEVICE_IDENTIFY_SERVICE_H
