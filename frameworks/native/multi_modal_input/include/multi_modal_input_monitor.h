/*
 *  Copyright (c) 2025 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef GAME_CONTROLLER_MULTI_MODAL_INPUT_MONITOR_H
#define GAME_CONTROLLER_MULTI_MODAL_INPUT_MONITOR_H

#include <singleton.h>
#include <i_input_device_listener.h>
#include "gamecontroller_constants.h"

namespace OHOS {
namespace GameController {
using namespace OHOS::MMI;

/**
 * Monitors events related to multi-mode input SA.
 */
class MultiModalInputMonitor : public DelayedSingleton<MultiModalInputMonitor> {
DECLARE_DELAYED_SINGLETON(MultiModalInputMonitor)

public:
    /**
     * Registering the multi-mode Input Listening Event By User.
     * @return true: success
     */
    bool RegisterMonitorByUser();

    /**
     * Registering the multi-mode Input Listening Event By System.
     */
    void RegisterMonitorBySystem();

    /**
     * Unregister the multi-mode input listening event By User.
     */
    void UnRegisterMonitorByUser();

    /**
     * Unregister the multi-mode input listening event By System.
     */
    void UnRegisterMonitorBySystem();

private:
    bool RegisterMonitor();

    void UnRegisterMonitor();

private:
    std::mutex monitorMute_;

    /**
     * Multi-mode input device listening callback
     */
    std::shared_ptr<IInputDeviceListener> inputDeviceListener_;

    bool isRegisterByUser_{false};

    bool isRegisterBySystem_{false};

    /**
     * Indicates the device registration result entered in multimode mode.
     */
    bool registerDeviceResult_{false};
};
}
}

#endif //GAME_CONTROLLER_MULTI_MODAL_INPUT_MONITOR_H
