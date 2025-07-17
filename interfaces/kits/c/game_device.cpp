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

#include "game_device.h"
#include "game_device_proxy.h"

GameController_ErrorCode OH_GameDevice_GetAllDeviceInfos(GameDevice_AllDeviceInfos** allDeviceInfos)
{
    return OHOS::GameController::GameDeviceProxy::GetInstance().GetAllDeviceInfos(allDeviceInfos);
}

GameController_ErrorCode OH_GameDevice_RegisterDeviceMonitor(GameDevice_DeviceMonitorCallback deviceMonitorCallback)
{
    return OHOS::GameController::GameDeviceProxy::GetInstance().RegisterDeviceMonitor(deviceMonitorCallback);
}

GameController_ErrorCode OH_GameDevice_UnregisterDeviceMonitor(void)
{
    return OHOS::GameController::GameDeviceProxy::GetInstance().UnRegisterDeviceMonitor();
}

GameController_ErrorCode OH_GameDevice_DestroyAllDeviceInfos(GameDevice_AllDeviceInfos** allDeviceInfos)
{
    return OHOS::GameController::GameDeviceProxy::GetInstance().DestroyAllDeviceInfos(allDeviceInfos);
}

GameController_ErrorCode OH_GameDevice_AllDeviceInfos_GetCount(
    const struct GameDevice_AllDeviceInfos* allDeviceInfos, int32_t* count)
{
    return OHOS::GameController::GameDeviceProxy::GetInstance().GetCountFromAllDeviceInfos(allDeviceInfos, count);
}

GameController_ErrorCode OH_GameDevice_AllDeviceInfos_GetDeviceInfo(
    const struct GameDevice_AllDeviceInfos* allDeviceInfos, const int32_t index,
    GameDevice_DeviceInfo** deviceInfo)
{
    return OHOS::GameController::GameDeviceProxy::GetInstance().GetDeviceInfoFromAllDeviceInfos(allDeviceInfos, index,
                                                                                                deviceInfo);
}

