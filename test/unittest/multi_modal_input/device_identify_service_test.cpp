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

#include <gtest/hwext/gtest-ext.h>
#include <gtest/hwext/gtest-tag.h>
#include <gmock/gmock-actions.h>
#include <gmock/gmock-spec-builders.h>

#define private public

#include "device_identify_service.h"
#include "gamecontroller_server_client.h"
#include "device_info_service.h"

#undef private

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include "refbase.h"
#include "gamecontroller_errors.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
namespace {
const int32_t DEVICE_ID1 = 1;
const int32_t DEVICE_ID2 = 2;
const int32_t DEVICE_ID3 = 3;
const int32_t DEVICE_ID4 = 4;
const int32_t DEVICE_ID5 = 5;
const int32_t ALPHABETIC_KEYBOARD_TYPE = 2; // Full keyboard
const int32_t INVALID_KEYBOARD_TYPE = 3;
const int32_t TOTAL_RESULT = 4;
}
class ServerClientMock : public GameControllerServerClient {
public:
    int32_t IdentifyDevice(const std::vector<DeviceInfo> &deviceInfos, std::vector<DeviceInfo> &identifyResult) override
    {
        for (const DeviceInfo &deviceInfo: identifyResult_) {
            identifyResult.push_back(deviceInfo);
        }
        return 0;
    }

public:
    int32_t result_;
    std::vector<DeviceInfo> identifyResult_;
};

class DeviceInfoMock : public DeviceInfoService {
public:
    std::pair<int32_t, int32_t> GetKeyBoardType(int32_t id) override
    {
        if (keyBoardTypeMap_.count(id)) {
            return keyBoardTypeMap_[id];
        }
        return std::pair<int32_t, int32_t>(GAME_ERR_CALL_MULTI_INPUT_FAIL,
                                           ALPHABETIC_KEYBOARD_TYPE);
    }

public:
    std::unordered_map<int32_t, std::pair<int32_t, int32_t>> keyBoardTypeMap_;
};

class DeviceIdentifyServiceTest : public testing::Test {
public:
    void SetUp() override;

    void TearDown() override;

public:
    std::shared_ptr<ServerClientMock> serverClientMock_;
    std::shared_ptr<DeviceInfoMock> deviceInfoServiceMock_;
};

void DeviceIdentifyServiceTest::SetUp()
{
    serverClientMock_ = std::make_shared<ServerClientMock>();
    GameControllerServerClient::instance_ = serverClientMock_;
    deviceInfoServiceMock_ = std::make_shared<DeviceInfoMock>();
    DeviceInfoService::instance_ = deviceInfoServiceMock_;
}

void DeviceIdentifyServiceTest::TearDown()
{
    serverClientMock_.reset();
    GameControllerServerClient::instance_ = nullptr;
    deviceInfoServiceMock_.reset();
    DeviceInfoService::instance_ = nullptr;
}

/**
* @tc.name: GetAllDeviceInfos_001
* @tc.desc: If req is empty, the result is empty.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(DeviceIdentifyServiceTest, IdentifyDeviceType_001, TestSize.Level0)
{
    std::vector<DeviceInfo> req;
    std::vector<DeviceInfo> result = DelayedSingleton<DeviceIdentifyService>::GetInstance()->IdentifyDeviceType(req);
    ASSERT_EQ(0, result.size());
}

DeviceInfo BuildDeviceInfo(int32_t deviceId, InputSourceTypeEnum sourceType)
{
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "uniq_" + std::to_string(deviceId);
    deviceInfo.deviceType = DeviceTypeEnum::UNKNOWN;
    deviceInfo.ids.insert(deviceId);
    deviceInfo.sourceTypeSet.insert(sourceType);
    deviceInfo.idSourceTypeMap[deviceId] = deviceInfo.sourceTypeSet;
    return deviceInfo;
}

void CheckResult(const std::vector<DeviceInfo> &deviceResult)
{
    int32_t idx = DEVICE_ID1;
    for (const auto &device: deviceResult) {
        ASSERT_EQ(1, device.ids.count(idx));
        if (idx == DEVICE_ID1) {
            ASSERT_EQ(DeviceTypeEnum::GAME_MOUSE, device.deviceType);
        } else {
            ASSERT_EQ(DeviceTypeEnum::UNKNOWN, device.deviceType);
        }

        if (idx == DEVICE_ID2) {
            ASSERT_TRUE(device.hasFullKeyBoard);
        } else {
            ASSERT_FALSE(device.hasFullKeyBoard);
        }
        idx++;
    }
}

HWTEST_F(DeviceIdentifyServiceTest, IdentifyDeviceType_002, TestSize.Level0)
{
    std::vector<DeviceInfo> req;
    std::vector<DeviceInfo> result;
    DeviceInfo deviceInfo1 = BuildDeviceInfo(DEVICE_ID1, InputSourceTypeEnum::MOUSE);
    req.push_back(deviceInfo1);
    DeviceInfo result1 = deviceInfo1;
    result1.deviceType = DeviceTypeEnum::GAME_MOUSE;
    result.push_back(result1);

    // keyboard type is full keyboard
    DeviceInfo deviceInfo2 = BuildDeviceInfo(DEVICE_ID2, InputSourceTypeEnum::KEYBOARD);
    req.push_back(deviceInfo2);
    DeviceInfo result2 = deviceInfo2;
    result2.deviceType = DeviceTypeEnum::UNKNOWN;
    result.push_back(result2);
    deviceInfoServiceMock_->keyBoardTypeMap_[DEVICE_ID2] = std::pair<int32_t, int32_t>(GAME_CONTROLLER_SUCCESS,
                                                                                       ALPHABETIC_KEYBOARD_TYPE);

    // keyboard type is not full keyboard
    DeviceInfo deviceInfo3 = BuildDeviceInfo(DEVICE_ID3, InputSourceTypeEnum::KEYBOARD);
    req.push_back(deviceInfo3);
    DeviceInfo result3 = deviceInfo3;
    result3.deviceType = DeviceTypeEnum::UNKNOWN;
    result.push_back(result3);
    deviceInfoServiceMock_->keyBoardTypeMap_[DEVICE_ID3] = std::pair<int32_t, int32_t>(GAME_CONTROLLER_SUCCESS,
                                                                                       INVALID_KEYBOARD_TYPE);

    // get keyboard type failed
    DeviceInfo deviceInfo4 = BuildDeviceInfo(DEVICE_ID4, InputSourceTypeEnum::KEYBOARD);
    req.push_back(deviceInfo4);
    DeviceInfo result4 = deviceInfo4;
    result4.deviceType = DeviceTypeEnum::UNKNOWN;
    result.push_back(result4);
    deviceInfoServiceMock_->keyBoardTypeMap_[DEVICE_ID4] = std::pair<int32_t, int32_t>(GAME_ERR_CALL_MULTI_INPUT_FAIL,
                                                                                       ALPHABETIC_KEYBOARD_TYPE);

    DeviceInfo deviceInfo5 = BuildDeviceInfo(DEVICE_ID5, InputSourceTypeEnum::JOYSTICK);
    req.push_back(deviceInfo5);

    serverClientMock_->result_ = 0;
    serverClientMock_->identifyResult_ = result;

    std::vector<DeviceInfo> deviceResult = DelayedSingleton<DeviceIdentifyService>::GetInstance()->IdentifyDeviceType(
        req);
    ASSERT_EQ(TOTAL_RESULT, deviceResult.size());
    CheckResult(deviceResult);
}
}
}