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
#include "gamecontroller_server_client_mock.h"

#undef private

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include "refbase.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
class DeviceIdentifyServiceTest : public testing::Test {
public:
    void SetUp() override;

    void TearDown() override;

public:
    std::shared_ptr<GameControllerServerClientMock> gameControllerServerClientMock_;
};

void DeviceIdentifyServiceTest::SetUp()
{
    gameControllerServerClientMock_ = std::make_shared<GameControllerServerClientMock>();
    GameControllerServerClient::instance_ = gameControllerServerClientMock_;
    DelayedSingleton<DeviceIdentifyService>::GetInstance()->identifyResultMap_.clear();
}

void DeviceIdentifyServiceTest::TearDown()
{
    gameControllerServerClientMock_.reset();
    GameControllerServerClient::instance_ = nullptr;
    DelayedSingleton<DeviceIdentifyService>::GetInstance()->identifyResultMap_.clear();
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

/**
* @tc.name: GetAllDeviceInfos_002
* @tc.desc: If the IdentifyDevice interface returns a failure message,
 * the system obtains the device type from the local cache.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(DeviceIdentifyServiceTest, IdentifyDeviceType_002, TestSize.Level0)
{
    DelayedSingleton<DeviceIdentifyService>::GetInstance()->identifyResultMap_["uniq1"] = DeviceTypeEnum::GAME_PAD;
    std::vector<DeviceInfo> req;
    DeviceInfo deviceInfo1;
    deviceInfo1.uniq = "uniq1";
    deviceInfo1.deviceType = DeviceTypeEnum::UNKNOWN;
    req.push_back(deviceInfo1);
    DeviceInfo deviceInfo2;
    deviceInfo2.uniq = "uniq2";
    deviceInfo2.deviceType = DeviceTypeEnum::UNKNOWN;
    req.push_back(deviceInfo2);
    EXPECT_CALL(*(gameControllerServerClientMock_.get()), IdentifyDevice(testing::_, testing::_)).WillOnce(Return(1));

    std::vector<DeviceInfo> result = DelayedSingleton<DeviceIdentifyService>::GetInstance()->IdentifyDeviceType(req);

    ASSERT_EQ(DeviceTypeEnum::GAME_PAD, result.at(0).deviceType);
    ASSERT_EQ(DeviceTypeEnum::UNKNOWN, result.at(1).deviceType);
}
}
}