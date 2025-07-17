/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "game_device_event_proxy.h"
#include "game_device_proxy.h"
#include "gamecontroller_log.h"
#include "gamecontroller_client_model.h"
#include "gamecontroller_utils.h"

namespace OHOS {
namespace GameController {
GameDeviceEventProxy::GameDeviceEventProxy()
{
}

GameDeviceEventProxy::~GameDeviceEventProxy()
{
}

GameController_ErrorCode GameDeviceEventProxy::GetChangedType(
    const struct GameDevice_DeviceEvent* deviceEvent, GameDevice_StatusChangedType* statusChangedType)
{
    if (deviceEvent == nullptr) {
        HILOGE("[CAPI][DeviceEvent_GetChangedType]deviceEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    int32_t changeType = ((GameDeviceEvent*)deviceEvent)->changeType;
    *statusChangedType = static_cast<GameDevice_StatusChangedType>(changeType);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GameDeviceEventProxy::GetDeviceInfo(const struct GameDevice_DeviceEvent* deviceEvent,
                                                             GameDevice_DeviceInfo** deviceInfo)
{
    if (deviceEvent == nullptr) {
        HILOGE("[CAPI][DeviceEvent_GetDeviceInfo]deviceEvent is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    BasicDeviceInfo deviceInfoTemp = ((GameDeviceEvent*)deviceEvent)->deviceInfo;
    BasicDeviceInfo* deviceInfoPointer = new BasicDeviceInfo();
    deviceInfoPointer->uniq = deviceInfoTemp.uniq;
    deviceInfoPointer->name = deviceInfoTemp.name;
    deviceInfoPointer->product = deviceInfoTemp.product;
    deviceInfoPointer->vendor = deviceInfoTemp.vendor;
    deviceInfoPointer->version = deviceInfoTemp.version;
    deviceInfoPointer->phys = deviceInfoTemp.phys;
    deviceInfoPointer->deviceType = deviceInfoTemp.deviceType;
    *deviceInfo = ((GameDevice_DeviceInfo*)deviceInfoPointer);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GameDeviceEventProxy::DestroyDeviceInfo(GameDevice_DeviceInfo** deviceInfo)
{
    if (deviceInfo == nullptr) {
        HILOGE("[CAPI][DestroyDeviceInfo]deviceInfo is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    delete ((BasicDeviceInfo*)*deviceInfo);
    *deviceInfo = nullptr;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GameDeviceEventProxy::GetDeviceIdFromDeviceInfo(
    const struct GameDevice_DeviceInfo* deviceInfo, char** deviceId)
{
    if (deviceInfo == nullptr || deviceId == nullptr) {
        HILOGE("[CAPI][GetDeviceIdFromDeviceInfo]deviceInfo or deviceId is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    int32_t ret = StringUtils::ConvertToCharPtrArray(((BasicDeviceInfo*)deviceInfo)->uniq, deviceId);
    if (ret == GAME_CONTROLLER_SUCCESS) {
        return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
    }
    return GameController_ErrorCode::GAME_CONTROLLER_NO_MEMORY;
}

GameController_ErrorCode GameDeviceEventProxy::GetNameFromDeviceInfo(
    const struct GameDevice_DeviceInfo* deviceInfo, char** name)
{
    if (deviceInfo == nullptr || name == nullptr) {
        HILOGE("[CAPI][GetNameFromDeviceInfo]deviceInfo or name is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }

    int32_t ret = StringUtils::ConvertToCharPtrArray(((BasicDeviceInfo*)deviceInfo)->name, name);
    if (ret == GAME_CONTROLLER_SUCCESS) {
        return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
    }
    return GameController_ErrorCode::GAME_CONTROLLER_NO_MEMORY;
}

GameController_ErrorCode GameDeviceEventProxy::GetProductFromDeviceInfo(
    const struct GameDevice_DeviceInfo* deviceInfo, int32_t* product)
{
    if (deviceInfo == nullptr) {
        HILOGE("[CAPI][GetProductFromDeviceInfo]deviceInfo is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *product = ((BasicDeviceInfo*)deviceInfo)->product;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GameDeviceEventProxy::GetVersionFromDeviceInfo(
    const struct GameDevice_DeviceInfo* deviceInfo, int32_t* version)
{
    if (deviceInfo == nullptr) {
        HILOGE("[CAPI][GetVersionFromDeviceInfo]deviceInfo is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *version = ((BasicDeviceInfo*)deviceInfo)->version;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GameDeviceEventProxy::GetPhysFromDeviceInfo(
    const struct GameDevice_DeviceInfo* deviceInfo, char** phys)
{
    if (deviceInfo == nullptr || phys == nullptr) {
        HILOGE("[CAPI][GetPhysFromDeviceInfo]deviceInfo or phys is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }

    int32_t ret = StringUtils::ConvertToCharPtrArray(((BasicDeviceInfo*)deviceInfo)->phys, phys);
    if (ret == GAME_CONTROLLER_SUCCESS) {
        return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
    }
    return GameController_ErrorCode::GAME_CONTROLLER_NO_MEMORY;
}

GameController_ErrorCode GameDeviceEventProxy::GetDeviceTypeFromDeviceInfo(
    const struct GameDevice_DeviceInfo* deviceInfo,
    GameDevice_DeviceType* deviceType)
{
    if (deviceInfo == nullptr) {
        HILOGE("[CAPI][GetDeviceTypeFromDeviceInfo]deviceInfo is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    int32_t type = ((BasicDeviceInfo*)deviceInfo)->deviceType;
    *deviceType = static_cast<GameDevice_DeviceType>(type);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}
}
}