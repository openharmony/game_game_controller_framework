/*
 *  Copyright (c) 2025 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "input_device_listener.h"
#include "gamecontroller_log.h"
#include "multi_modal_input_mgt_service.h"
#include "gamecontroller_constants.h"

namespace OHOS {
namespace GameController {
InputDeviceListener::InputDeviceListener()
{
}

InputDeviceListener::~InputDeviceListener()
{
}

void InputDeviceListener::OnDeviceAdded(int32_t deviceId, const std::string &type)
{
    HILOGI("[GameController]OnDeviceAdded. deviceId is %{public}d, type is %{public}s", deviceId,
           type.c_str());
    HandleEvent(deviceId, DeviceChangeType::ADD);
}

void InputDeviceListener::OnDeviceRemoved(int32_t deviceId, const std::string &type)
{
    HILOGI("[GameController]OnDeviceRemoved. deviceId is %{public}d, type is %{public}s", deviceId,
           type.c_str());
    HandleEvent(deviceId, DeviceChangeType::REMOVE);
}

void InputDeviceListener::HandleEvent(int32_t deviceId, DeviceChangeType deviceChangeType)
{
    DeviceChangeEvent deviceEvent;
    deviceEvent.deviceId = deviceId;
    deviceEvent.deviceChangeType = deviceChangeType;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->DelayHandleDeviceChangeEvent(deviceEvent);
}
}
}