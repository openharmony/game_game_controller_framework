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

#include <singleton.h>
#include "plugin_client.h"
#include "plugin_manager.h"
#include "plugin_callback_manager.h"

namespace OHOS {
namespace GameController {
void GameControllerPluginClient::UpdateCurrentAncoGameInfo(const AncoGameInfo &ancoGameInfo)
{
    GameInfo gameInfo;
    gameInfo.bundleName = ancoGameInfo.bundleName;
    gameInfo.windowId = ancoGameInfo.windowId;
    DelayedSingleton<PluginManager>::GetInstance()->UpdateCurrentGameInfo(gameInfo, ancoGameInfo.isEnableKeyMapping);
}

void GameControllerPluginClient::UpdateCurrentAncoWindowInfo(const AncoGameWindowInfo &ancoGameWindowInfo)
{
    WindowInfoEntity windowInfoEntity{};
    windowInfoEntity.bundleName = ancoGameWindowInfo.bundleName;
    windowInfoEntity.windowId = ancoGameWindowInfo.windowId;
    windowInfoEntity.maxWidth = ancoGameWindowInfo.maxWidth;
    windowInfoEntity.maxHeight = ancoGameWindowInfo.maxHeight;
    windowInfoEntity.currentWidth = ancoGameWindowInfo.currentWidth;
    windowInfoEntity.currentHeight = ancoGameWindowInfo.currentHeight;
    windowInfoEntity.xPosition = ancoGameWindowInfo.xPosition;
    windowInfoEntity.yPosition = ancoGameWindowInfo.yPosition;
    windowInfoEntity.isFullScreen = ancoGameWindowInfo.isFullScreen;
    windowInfoEntity.xCenter = ancoGameWindowInfo.currentWidth / HALF_LENGTH;
    windowInfoEntity.yCenter = ancoGameWindowInfo.currentHeight / HALF_LENGTH;
    windowInfoEntity.displayId = ancoGameWindowInfo.displayId;
    windowInfoEntity.isPluginMode = true;
    DelayedSingleton<PluginManager>::GetInstance()->UpdateCurrentWindowInfo(windowInfoEntity);
}

void GameControllerPluginClient::ClearCurrentAncoGameInfo(const std::string &bundleName)
{
    DelayedSingleton<PluginManager>::GetInstance()->ClearCurrentGameInfo(bundleName);
}

bool GameControllerPluginClient::IsSupportKeyMapping(const std::string &bundleName)
{
    return DelayedSingleton<PluginManager>::GetInstance()->IsSupportKeyMapping(bundleName);
}

void GameControllerPluginClient::BroadcastDeviceInfo(const DeviceInfo &deviceInfo)
{
    DelayedSingleton<PluginManager>::GetInstance()->BroadcastDeviceInfo(deviceInfo);
}

void GameControllerPluginClient::RegisterDeviceCallBack(const std::shared_ptr<DeviceCallback> &deviceCallback)
{
    DelayedSingleton<PluginCallbackManager>::GetInstance()->RegisterDeviceCallBack(deviceCallback);
}

void GameControllerPluginClient::RegisterWindowOprCallback(const std::shared_ptr<WindowOprCallback> &windowOprCallback)
{
    DelayedSingleton<PluginCallbackManager>::GetInstance()->RegisterWindowOprCallback(windowOprCallback);
}

void GameControllerPluginClient::RegisterSaEventCallback(const std::shared_ptr<SaEventCallback> &saEventCallback)
{
    DelayedSingleton<PluginCallbackManager>::GetInstance()->RegisterSaEventCallback(saEventCallback);
}

void GameControllerPluginClient::SetFocusStatus(const std::string &bundleName, bool isFocus)
{
}
}
}