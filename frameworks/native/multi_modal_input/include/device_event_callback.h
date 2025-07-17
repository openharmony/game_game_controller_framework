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

#ifndef GAME_CONTROLLER_FRAMEWORK_DEVICE_EVENT_CALLBACK_H
#define GAME_CONTROLLER_FRAMEWORK_DEVICE_EVENT_CALLBACK_H

#include <singleton.h>
#include <unordered_map>
#include "gamecontroller_client_model.h"
#include "ffrt.h"

namespace OHOS {
namespace GameController {
/**
 * Manages device event callbacks.
 */
class DeviceEventCallback : public DelayedSingleton<DeviceEventCallback> {
DECLARE_DELAYED_SINGLETON(DeviceEventCallback);
public:
    /**
     * Registering a Device Callback Event
     * @param apiTypeEnum API Source
     * @param callback Callback Method
     */
    void RegisterGameDeviceEventCallback(const ApiTypeEnum apiTypeEnum,
                                         const std::shared_ptr<DeviceEventCallbackBase> &callback);

    /**
     * Device unregistration callback event
     * @param apiTypeEnum API Source
     */
    void UnRegisterGameDeviceEventCallback(const ApiTypeEnum apiTypeEnum);

    /**
     * Performs device event callback.
     * @param deviceEvent Device Event
     */
    void OnDeviceEventCallback(const DeviceEvent &deviceEvent);

private:
    /**
     * Device event callback lock
     */
    std::mutex deviceEventCallbackMute_;

    /**
     * Cache for device event callback methods
     */
    std::shared_ptr<DeviceEventCallbackBase> deviceEventCallback_;

    /**
     * Device online and offline processing queue
     */
    std::unique_ptr<ffrt::queue> deviceQueryQueue_{nullptr};
};
}
}

#endif //GAME_CONTROLLER_FRAMEWORK_DEVICE_EVENT_CALLBACK_H
