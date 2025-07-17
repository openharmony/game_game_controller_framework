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

#include "game_device_client.h"
#include "device_event_callback.h"
#include "multi_modal_input_mgt_service.h"

namespace OHOS {
namespace GameController {
void GameDeviceClient::RegisterGameDeviceEventCallback(const ApiTypeEnum apiTypeEnum,
                                                       const std::shared_ptr<DeviceEventCallbackBase> &callback)
{
    DelayedSingleton<DeviceEventCallback>::GetInstance()->RegisterGameDeviceEventCallback(apiTypeEnum,
                                                                                          callback);
}

void GameDeviceClient::UnRegisterGameDeviceEventCallback(const ApiTypeEnum apiTypeEnum)
{
    DelayedSingleton<DeviceEventCallback>::GetInstance()->UnRegisterGameDeviceEventCallback(
        apiTypeEnum);
}

std::pair<int32_t, std::vector<DeviceInfo>> GameDeviceClient::GetAllDeviceInfos()
{
    return DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetAllDeviceInfos();
}
}
}