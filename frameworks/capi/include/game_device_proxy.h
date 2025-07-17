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

#ifndef GAME_CONTROLLER_FRAMEWORK_GAME_DEVICE_PROXY_H
#define GAME_CONTROLLER_FRAMEWORK_GAME_DEVICE_PROXY_H

#include <singleton.h>
#include <vector>
#include "game_device_event.h"
#include "game_controller_type.h"
#include "game_device.h"
#include "gamecontroller_client_model.h"

namespace OHOS {
namespace GameController {
struct AllDeviceInfos {
    /**
     * List of all devices
     */
    std::vector<BasicDeviceInfo> allDeviceInfos;

    int32_t count;
};

/**
 * Game Device change event
 */
struct GameDeviceEvent {
    /**
     * Event type, which can be online or offline.
     */
    int32_t changeType;

    /**
     * Device Information
     */
    BasicDeviceInfo deviceInfo;
};

class GameDeviceEventCallback : public DeviceEventCallbackBase {
public:
    GameDeviceEventCallback() = default;

    ~GameDeviceEventCallback() = default;

    int32_t OnDeviceEventCallback(const DeviceEvent &event) override;

    void SetCallback(GameDevice_DeviceMonitorCallback callback);

private:
    GameDevice_DeviceMonitorCallback callback_ = nullptr;
};

class GameDeviceProxy : public Singleton<GameDeviceProxy> {
DECLARE_SINGLETON(GameDeviceProxy);
public:

    GameController_ErrorCode GetAllDeviceInfos(GameDevice_AllDeviceInfos** allDeviceInfos);

    GameController_ErrorCode RegisterDeviceMonitor(
        GameDevice_DeviceMonitorCallback deviceMonitorCallback);

    GameController_ErrorCode UnRegisterDeviceMonitor();

    GameController_ErrorCode DestroyAllDeviceInfos(GameDevice_AllDeviceInfos** allDeviceInfos);

    GameController_ErrorCode GetCountFromAllDeviceInfos(
        const struct GameDevice_AllDeviceInfos* allDeviceInfos,
        int32_t* count);

    GameController_ErrorCode GetDeviceInfoFromAllDeviceInfos(
        const struct GameDevice_AllDeviceInfos* allDeviceInfos,
        const int32_t index,
        GameDevice_DeviceInfo** deviceInfo);
};
}
}

#endif //GAME_CONTROLLER_FRAMEWORK_GAME_DEVICE_PROXY_H
