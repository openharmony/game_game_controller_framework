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
#include "game_device_event.h"
#include "game_device_event_proxy.h"

GameController_ErrorCode OH_GameDevice_DeviceEvent_GetChangedType(
    const struct GameDevice_DeviceEvent* deviceEvent,
    GameDevice_StatusChangedType* statusChangedType)
{
    return OHOS::GameController::GameDeviceEventProxy::GetInstance().GetChangedType(deviceEvent, statusChangedType);
}

GameController_ErrorCode OH_GameDevice_DeviceEvent_GetDeviceInfo(
    const struct GameDevice_DeviceEvent* deviceEvent,
    GameDevice_DeviceInfo** deviceInfo)
{
    return OHOS::GameController::GameDeviceEventProxy::GetInstance().GetDeviceInfo(deviceEvent, deviceInfo);
}

GameController_ErrorCode OH_GameDevice_DestroyDeviceInfo(GameDevice_DeviceInfo** deviceInfo)
{
    return OHOS::GameController::GameDeviceEventProxy::GetInstance().DestroyDeviceInfo(deviceInfo);
}

GameController_ErrorCode OH_GameDevice_DeviceInfo_GetDeviceId(const struct GameDevice_DeviceInfo* deviceInfo,
                                                              char** deviceId)
{
    return OHOS::GameController::GameDeviceEventProxy::GetInstance().GetDeviceIdFromDeviceInfo(deviceInfo, deviceId);
}

GameController_ErrorCode OH_GameDevice_DeviceInfo_GetName(const struct GameDevice_DeviceInfo* deviceInfo,
                                                          char** name)
{
    return OHOS::GameController::GameDeviceEventProxy::GetInstance().GetNameFromDeviceInfo(deviceInfo, name);
}

GameController_ErrorCode OH_GameDevice_DeviceInfo_GetProduct(const struct GameDevice_DeviceInfo* deviceInfo,
                                                             int32_t* product)
{
    return OHOS::GameController::GameDeviceEventProxy::GetInstance().GetProductFromDeviceInfo(deviceInfo, product);
}

GameController_ErrorCode OH_GameDevice_DeviceInfo_GetVersion(const struct GameDevice_DeviceInfo* deviceInfo,
                                                             int32_t* version)
{
    return OHOS::GameController::GameDeviceEventProxy::GetInstance().GetVersionFromDeviceInfo(deviceInfo, version);
}

GameController_ErrorCode OH_GameDevice_DeviceInfo_GetPhysicalAddress(const struct GameDevice_DeviceInfo* deviceInfo,
                                                                     char** physicalAddress)
{
    return OHOS::GameController::GameDeviceEventProxy::GetInstance().GetPhysFromDeviceInfo(deviceInfo, physicalAddress);
}

GameController_ErrorCode OH_GameDevice_DeviceInfo_GetDeviceType(const struct GameDevice_DeviceInfo* deviceInfo,
                                                                GameDevice_DeviceType* deviceType)
{
    return OHOS::GameController::GameDeviceEventProxy::GetInstance().GetDeviceTypeFromDeviceInfo(deviceInfo,
                                                                                                 deviceType);
}