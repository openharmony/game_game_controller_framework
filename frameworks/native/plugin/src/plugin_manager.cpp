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

#include "plugin_manager.h"
#include "key_to_touch_manager.h"
#include "key_mapping_service.h"
#include "plugin_callback_manager.h"

namespace OHOS {
namespace GameController {
PluginManager::PluginManager()
{
}

PluginManager::~PluginManager()
{
}

void PluginManager::UpdateCurrentGameInfo(const GameInfo &gameInfo, bool isEnableKeyMapping)
{
    std::lock_guard<ffrt::mutex> lock(locker_);
    HILOGI("UpdateCurrentGameInfo: bundleName is [%{public}s], isEnableKeyMapping[%{public}d]",
           gameInfo.bundleName.c_str(), isEnableKeyMapping);
    bundleName_ = gameInfo.bundleName;
    DelayedSingleton<KeyMappingService>::GetInstance()->IsSupportGameKeyMapping(bundleName_, "");
    DelayedSingleton<KeyMappingService>::GetInstance()->SetWindowId(gameInfo.windowId);
    DelayedSingleton<KeyToTouchManager>::GetInstance()->SetCurrentBundleName(bundleName_, isEnableKeyMapping, true);
    DelayedSingleton<KeyMappingService>::GetInstance()->UpdateGameKeyMappingWhenTemplateChange(
        bundleName_, DeviceTypeEnum::GAME_KEY_BOARD);
}

void PluginManager::UpdateCurrentWindowInfo(const WindowInfoEntity &windowInfoEntity)
{
    std::lock_guard<ffrt::mutex> lock(locker_);
    DelayedSingleton<KeyToTouchManager>::GetInstance()->UpdateWindowInfo(windowInfoEntity);
}

void PluginManager::ClearCurrentGameInfo(const std::string &bundleName)
{
    std::lock_guard<ffrt::mutex> lock(locker_);
    HILOGI("ClearCurrentGameInfo bundleName [%{public}s]", bundleName.c_str());
    bundleName_ = "";
    DelayedSingleton<KeyMappingService>::GetInstance()->ClearGameKeyMapping();
    DelayedSingleton<KeyToTouchManager>::GetInstance()->ClearGameKeyMapping();
}

bool PluginManager::IsSupportKeyMapping(const std::string &bundleName)
{
    return DelayedSingleton<KeyMappingService>::GetInstance()->CheckSupportKeyMapping(bundleName);
}

void PluginManager::BroadCastDeviceInfo(const DeviceInfo &deviceInfo)
{
    DelayedSingleton<KeyMappingService>::GetInstance()->BroadCastDeviceInfo(deviceInfo);
}

}
}