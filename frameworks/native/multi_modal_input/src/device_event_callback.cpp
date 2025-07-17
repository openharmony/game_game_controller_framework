
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

#include "device_event_callback.h"
#include "multi_modal_input_monitor.h"
#include "gamecontroller_log.h"
#include "multi_modal_input_mgt_service.h"

namespace OHOS {
namespace GameController {
DeviceEventCallback::DeviceEventCallback()
{
    deviceQueryQueue_ = std::make_unique<ffrt::queue>("deviceQueryQueue_",
                                                      ffrt::queue_attr().qos(ffrt::qos_default));
}

DeviceEventCallback::~DeviceEventCallback()
{
    deviceQueryQueue_ = nullptr;
    deviceEventCallback_ = nullptr;
}

void DeviceEventCallback::RegisterGameDeviceEventCallback(const ApiTypeEnum apiTypeEnum,
                                                          const std::shared_ptr<DeviceEventCallbackBase> &callback)
{
    bool isCallQueryDevice = false;
    {
        std::lock_guard<std::mutex> lock(deviceEventCallbackMute_);
        if (callback == nullptr) {
            return;
        }
        HILOGI("[DeviceEventCallback]RegisterGameDeviceEventCallback ApiTypeEnum is %{public}d", apiTypeEnum);
        if (deviceEventCallback_ == nullptr) {
            isCallQueryDevice = true;
        }
        deviceEventCallback_ = callback;

        DelayedSingleton<MultiModalInputMonitor>::GetInstance()->RegisterMonitorByUser();
    }

    if (isCallQueryDevice) {
        /*
         * To solve the problem that the system cannot receive event notifications
         * of online devices during registration listening, the system needs to query devices.
         */
        deviceQueryQueue_->submit([this] {
            HILOGI("[DeviceEventCallback]do GetAllDeviceInfosWhenRegisterDeviceMonitor");
            DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetAllDeviceInfosWhenRegisterDeviceMonitor();
        });
    }
}

void DeviceEventCallback::UnRegisterGameDeviceEventCallback(const ApiTypeEnum apiTypeEnum)
{
    std::lock_guard<std::mutex> lock(deviceEventCallbackMute_);
    HILOGI("[DeviceEventCallback]UnRegisterGameDeviceEventCallback ApiTypeEnum is %{public}d", apiTypeEnum);
    deviceEventCallback_ = nullptr;

    DelayedSingleton<MultiModalInputMonitor>::GetInstance()->UnRegisterMonitorByUser();
}

void DeviceEventCallback::OnDeviceEventCallback(const DeviceEvent &deviceEvent)
{
    std::shared_ptr<DeviceEventCallbackBase> callbackBase = nullptr;
    {
        std::lock_guard<std::mutex> lock(deviceEventCallbackMute_);
        if (deviceEventCallback_ == nullptr) {
            HILOGW("[DeviceEventCallback]OnDeviceEventCallback failed. no register deviceEventCallback");
            return;
        }
        callbackBase = deviceEventCallback_;
    }

    int32_t code = callbackBase->OnDeviceEventCallback(deviceEvent);
    if (code != 0) {
        HILOGW("[DeviceEventCallback]OnDeviceEventCallback failed. code is %{public}d", code);
    }
}
}
}