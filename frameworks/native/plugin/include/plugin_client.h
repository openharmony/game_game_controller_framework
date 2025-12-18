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

#ifndef GAME_CONTROLLER_PLUGIN_CLIENT_H
#define GAME_CONTROLLER_PLUGIN_CLIENT_H

#include <string>
#include <memory>
#include "plugin_event_callback.h"
#include "gamecontroller_client_model.h"

namespace OHOS {
namespace GameController {
struct AncoGameInfo {
    std::string bundleName;
    int32_t windowId = 0;
    bool isEnableKeyMapping = true;
};

struct AncoGameWindowInfo {
    std::string bundleName;
    int32_t windowId = 0;
    int32_t displayId = 0;
    bool isFullScreen = false;
    int32_t xPosition = 0;
    int32_t yPosition = 0;
    int32_t currentWidth = 0;
    int32_t currentHeight = 0;
    int32_t maxWidth = 0;
    int32_t maxHeight = 0;
};

class GameControllerPluginClient {
public:
    static void UpdateCurrentAncoGameInfo(const AncoGameInfo &ancoGameInfo);

    static void UpdateCurrentAncoWindowInfo(const AncoGameWindowInfo &ancoGameWindowInfo);

    static void ClearCurrentAncoGameInfo(const std::string &bundleName);

    static bool IsSupportKeyMapping(const std::string &bundleName);

    static void BroadcastDeviceInfo(const DeviceInfo &deviceInfo);

    static void RegisterDeviceCallBack(const std::shared_ptr<DeviceCallback> &deviceCallback);

    static void RegisterWindowOprCallback(const std::shared_ptr<WindowOprCallback> &windowOprCallback);

    static void RegisterSaEventCallback(const std::shared_ptr<SaEventCallback> &saEventCallback);

    static void SetFocusStatus(const std::string &bundleName, bool isFocus);

private:
    static std::string currentBundleName_;
};
}
}

#endif //GAME_CONTROLLER_PLUGIN_CLIENT_H
