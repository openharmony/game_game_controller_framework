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
#ifndef GAME_CONTROLLER_FRAMEWORK_PLUGIN_MANAGER_H
#define GAME_CONTROLLER_FRAMEWORK_PLUGIN_MANAGER_H

#include <singleton.h>
#include <cpp/mutex.h>
#include "plugin_event_callback.h"
#include "gamecontroller_keymapping_model.h"
#include "key_to_touch_handler.h"

namespace OHOS {
namespace GameController {
class PluginManager : public DelayedSingleton<PluginManager> {
DECLARE_DELAYED_SINGLETON(PluginManager)

public:
    void UpdateCurrentGameInfo(const GameInfo &gameInfo, bool isEnableKeyMapping);

    void UpdateCurrentWindowInfo(const WindowInfoEntity &windowInfoEntity);

    void ClearCurrentGameInfo(const std::string &bundleName);

    bool IsSupportKeyMapping(const std::string &bundleName);

    void BroadCastDeviceInfo(const DeviceInfo &deviceInfo);

private:
    ffrt::mutex locker_;
    bool isPluginMode_{false};
    std::string bundleName_;
};
}
}

#endif //GAME_CONTROLLER_FRAMEWORK_PLUGIN_MANAGER_H
