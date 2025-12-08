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
#ifndef GAME_CONTROLLER_FRAMEWORK_PLUGIN_CALLBACK_MANAGER_H
#define GAME_CONTROLLER_FRAMEWORK_PLUGIN_CALLBACK_MANAGER_H

#include <string>
#include <singleton.h>
#include <cpp/queue.h>
#include <cpp/mutex.h>
#include "gamecontroller_client_model.h"
#include "plugin_event_callback.h"

namespace OHOS {
namespace GameController {
class PluginCallbackManager : public DelayedSingleton<PluginCallbackManager> {
DECLARE_DELAYED_SINGLETON(PluginCallbackManager)

public:
    void SetPluginMode(bool isPluginMode);

    void RegisterWindowOprCallback(const std::shared_ptr<WindowOprCallback> &windowOprCallback);

    void RegisterDeviceCallBack(const std::shared_ptr<DeviceCallback> &deviceCallback);

    void RegisterSaEventCallback(const std::shared_ptr<SaEventCallback> &saEventCallback);

    void OnDeviceStatusChanged(const std::string &bundleName, const DeviceInfo &deviceInfo);

    void HandleKeyMappingEnableChangeEvent(const std::string &bundleName, bool isEnabled);

    void SendInputEvent(const std::string &bundleName, const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                        bool isSimulate);

    void SendInputEvent(const std::string &bundleName, const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                        bool isSimulate);

    void RecoverGestureBackStatus(const std::string &bundleName);

    void DisableGestureBack(const std::string &bundleName);

    void SetTitleAndDockHoverHidden(const std::string &bundleName);

    void SetPointerVisible(const std::string &bundleName, bool visible);

private:
    ffrt::mutex locker_;
    std::unique_ptr<ffrt::queue> callbackQueue_{nullptr};
    std::string bundleName_;
    bool isPluginMode_{false};
    std::shared_ptr<WindowOprCallback> windowOprCallbackPtr_{nullptr};
    std::shared_ptr<DeviceCallback> deviceCallbackPtr_{nullptr};
    std::shared_ptr<SaEventCallback> saEventCallbackPtr_{nullptr};
};
}
}
#endif //GAME_CONTROLLER_FRAMEWORK_PLUGIN_CALLBACK_MANAGER_H
