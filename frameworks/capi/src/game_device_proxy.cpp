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

#include "game_device_proxy.h"
#include "gamecontroller_client_model.h"
#include "gamecontroller_log.h"
#include "gamecontroller_utils.h"
#include "gamecontroller_errors.h"
#include "game_device_client.h"

namespace OHOS {
namespace GameController {
GameDeviceProxy::GameDeviceProxy()
{
}

GameDeviceProxy::~GameDeviceProxy()
{
}

GameController_ErrorCode GameDeviceProxy::GetAllDeviceInfos(GameDevice_AllDeviceInfos** allDeviceInfos)
{
    std::pair<int32_t, std::vector<DeviceInfo>> pair =
        DelayedSingleton<GameDeviceClient>::GetInstance()->GetAllDeviceInfos();
    if (pair.first != GAME_CONTROLLER_SUCCESS) {
        return GAME_CONTROLLER_MULTIMODAL_INPUT_ERROR;
    }
    AllDeviceInfos* allDeviceInfosPointer = new AllDeviceInfos();
    std::vector<BasicDeviceInfo> infos;
    for (auto &info: pair.second) {
        infos.push_back(info.ConvertToBasicDeviceInfo());
    }
    allDeviceInfosPointer->allDeviceInfos = infos;
    allDeviceInfosPointer->count = static_cast<int32_t>(pair.second.size());
    *allDeviceInfos = ((GameDevice_AllDeviceInfos*)allDeviceInfosPointer);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GameDeviceProxy::RegisterDeviceMonitor(
    GameDevice_DeviceMonitorCallback deviceMonitorCallback)
{
    if (deviceMonitorCallback == nullptr) {
        HILOGE("[CAPI][RegisterDeviceMonitor]deviceMonitorCallback is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    std::shared_ptr<GameDeviceEventCallback> apiCallback = std::make_shared<GameDeviceEventCallback>();
    apiCallback->SetCallback(deviceMonitorCallback);
    GameDeviceClient::RegisterGameDeviceEventCallback(ApiTypeEnum::CAPI, apiCallback);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GameDeviceProxy::UnRegisterDeviceMonitor()
{
    GameDeviceClient::UnRegisterGameDeviceEventCallback(ApiTypeEnum::CAPI);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GameDeviceProxy::DestroyAllDeviceInfos(
    GameDevice_AllDeviceInfos** allDeviceInfos)
{
    if (allDeviceInfos == nullptr) {
        HILOGE("[CAPI][DestroyAllDeviceInfos]allDeviceInfos is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    delete ((AllDeviceInfos*)*allDeviceInfos);
    *allDeviceInfos = nullptr;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GameDeviceProxy::GetCountFromAllDeviceInfos(
    const struct GameDevice_AllDeviceInfos* allDeviceInfos,
    int32_t* count)
{
    if (allDeviceInfos == nullptr) {
        HILOGE("[CAPI][GetCountFromAllDeviceInfos]allDeviceInfos is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    *count = ((AllDeviceInfos*)allDeviceInfos)->count;
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

GameController_ErrorCode GameDeviceProxy::GetDeviceInfoFromAllDeviceInfos(
    const struct GameDevice_AllDeviceInfos* allDeviceInfos,
    const int32_t index, GameDevice_DeviceInfo** deviceInfo)
{
    if (allDeviceInfos == nullptr) {
        HILOGE("[CAPI][GetDeviceInfoFromAllDeviceInfos]allDeviceInfos is nullptr");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }
    int count = ((AllDeviceInfos*)allDeviceInfos)->count;
    if (index >= count || index < 0) {
        HILOGE("[CAPI][GetDeviceInfoFromAllDeviceInfos]index is out of range");
        return GameController_ErrorCode::GAME_CONTROLLER_PARAM_ERROR;
    }

    BasicDeviceInfo deviceInfoByIndex = ((AllDeviceInfos*)allDeviceInfos)->allDeviceInfos[index];
    BasicDeviceInfo* deviceInfoPointer = new BasicDeviceInfo();
    deviceInfoPointer->uniq = deviceInfoByIndex.uniq;
    deviceInfoPointer->name = deviceInfoByIndex.name;
    deviceInfoPointer->product = deviceInfoByIndex.product;
    deviceInfoPointer->vendor = deviceInfoByIndex.vendor;
    deviceInfoPointer->version = deviceInfoByIndex.version;
    deviceInfoPointer->phys = deviceInfoByIndex.phys;
    deviceInfoPointer->deviceType = deviceInfoByIndex.deviceType;
    *deviceInfo = ((GameDevice_DeviceInfo*)deviceInfoPointer);
    return GameController_ErrorCode::GAME_CONTROLLER_SUCCESS;
}

int32_t GameDeviceEventCallback::OnDeviceEventCallback(const DeviceEvent &event)
{
    if (callback_ == nullptr) {
        HILOGW("[capi]OnDeviceEventCallback failed. callback is nullptr");
        return GAME_ERR_ARGUMENT_NULL;
    }
    GameDeviceEvent* deviceEvent = new GameDeviceEvent();
    deviceEvent->changeType = event.changeType;
    deviceEvent->deviceInfo = event.deviceInfo.ConvertToBasicDeviceInfo();
    GameDevice_DeviceEvent* temp = (GameDevice_DeviceEvent*)deviceEvent;

    callback_(temp);

    if (deviceEvent != nullptr) {
        delete deviceEvent;
        deviceEvent = nullptr;
        temp = nullptr;
    }
    return GAME_CONTROLLER_SUCCESS;
}

void GameDeviceEventCallback::SetCallback(GameDevice_DeviceMonitorCallback callback)
{
    callback_ = callback;
}
}
}