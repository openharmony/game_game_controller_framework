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

#include <input_manager.h>
#include "multi_modal_input_monitor.h"
#include "input_device_listener.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
using namespace OHOS::MMI;
namespace {
const char* DEVICE_LISTENER_TYPE = "change";
}

MultiModalInputMonitor::MultiModalInputMonitor()
{
    inputDeviceListener_ = std::make_shared<InputDeviceListener>();
}

MultiModalInputMonitor::~MultiModalInputMonitor()
{
    inputDeviceListener_ = nullptr;
}

bool MultiModalInputMonitor::RegisterMonitorByUser()
{
    std::lock_guard<std::mutex> lock(monitorMute_);
    isRegisterByUser_ = true;
    return RegisterMonitor();
}

void MultiModalInputMonitor::RegisterMonitorBySystem()
{
    std::lock_guard<std::mutex> lock(monitorMute_);
    isRegisterBySystem_ = true;
    RegisterMonitor();
}

bool MultiModalInputMonitor::RegisterMonitor()
{
    if (registerDeviceResult_) {
        return true;
    }

    int32_t result = InputManager::GetInstance()->RegisterDevListener(DEVICE_LISTENER_TYPE,
                                                                      inputDeviceListener_);
    HILOGI("[MultiModalInput]RegisterDevListener is %{public}d", result);
    if (result != 0) {
        return false;
    }
    registerDeviceResult_ = true;
    return true;
}

void MultiModalInputMonitor::UnRegisterMonitorByUser()
{
    std::lock_guard<std::mutex> lock(monitorMute_);
    isRegisterByUser_ = false;
    UnRegisterMonitor();
}

void MultiModalInputMonitor::UnRegisterMonitorBySystem()
{
    std::lock_guard<std::mutex> lock(monitorMute_);
    isRegisterBySystem_ = false;
    UnRegisterMonitor();
}

void MultiModalInputMonitor::UnRegisterMonitor()
{
    if (!registerDeviceResult_) {
        return;
    }

    if (isRegisterBySystem_ || isRegisterByUser_) {
        return;
    }

    // Unregister device listener only when both the system and the user are deregistered.
    int32_t result = InputManager::GetInstance()->UnregisterDevListener(DEVICE_LISTENER_TYPE,
                                                                        inputDeviceListener_);
    HILOGI("[MultiModalInput]UnregisterDevListener Result is %{public}d", result);
    registerDeviceResult_ = false;
}
}
}