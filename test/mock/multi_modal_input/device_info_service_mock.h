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


#ifndef GAME_CONTROLLER_FRAMEWORK_DEVICE_INFO_SERVICE_MOCK_H
#define GAME_CONTROLLER_FRAMEWORK_DEVICE_INFO_SERVICE_MOCK_H

#include "device_info_service.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace GameController {
class DeviceInfoServiceMock : public DeviceInfoService {
public:
    MOCK_METHOD0(GetAllDeviceInfos, std::pair<int32_t, std::vector<InputDeviceInfo>>());
    
    MOCK_METHOD1(GetInputDeviceInfo, std::pair<int32_t, InputDeviceInfo>(int32_t deviceId));

    MOCK_METHOD1(GetKeyBoardType, std::pair<int32_t, int32_t>(int32_t id));
};
}
}
#endif //GAME_CONTROLLER_FRAMEWORK_DEVICE_INFO_SERVICE_MOCK_H
