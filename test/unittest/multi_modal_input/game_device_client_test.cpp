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

#include <gtest/hwext/gtest-ext.h>
#include <gtest/hwext/gtest-tag.h>
#include <gmock/gmock-actions.h>
#include <gmock/gmock-spec-builders.h>

#define private public

#include "device_event_callback.h"

#undef private

#include "game_device_client.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include "refbase.h"
#include "device_event_callback_impl.h"

using ::testing::Return;
using namespace testing::ext;

namespace OHOS {
namespace GameController {
class GameDeviceClientTest : public testing::Test {
public:
    void SetUp() override;

    void TearDown() override;
};

void GameDeviceClientTest::SetUp()
{
    DelayedSingleton<DeviceEventCallback>::GetInstance()->deviceEventCallback_ = nullptr;
}

void GameDeviceClientTest::TearDown()
{
    DelayedSingleton<DeviceEventCallback>::GetInstance()->deviceEventCallback_ = nullptr;
}

/**
* @tc.name: RegisterGameDeviceEventCallback_001
* @tc.desc: If the value of callback is nullptr, the system discards the callback and does not process the callback.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameDeviceClientTest, RegisterGameDeviceEventCallback_001, TestSize.Level0)
{
    std::shared_ptr<GameDeviceEventCallback> callback = std::make_shared<GameDeviceEventCallback>();
    DelayedSingleton<DeviceEventCallback>::GetInstance()->deviceEventCallback_ = callback;
    GameDeviceClient::RegisterGameDeviceEventCallback(ApiTypeEnum::CAPI, nullptr);
    ASSERT_TRUE(DelayedSingleton<DeviceEventCallback>::GetInstance()->deviceEventCallback_ == callback);
}

/**
* @tc.name: RegisterGameDeviceEventCallback_002
* @tc.desc: If the value of callback is not nullptr, the callback is cached to deviceEventCallback_.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameDeviceClientTest, RegisterGameDeviceEventCallback_002, TestSize.Level0)
{
    DelayedSingleton<DeviceEventCallback>::GetInstance()->deviceEventCallback_
        = std::make_shared<GameDeviceEventCallback>();
    std::shared_ptr<GameDeviceEventCallback> callback = std::make_shared<GameDeviceEventCallback>();
    GameDeviceClient::RegisterGameDeviceEventCallback(ApiTypeEnum::CAPI, callback);
    ASSERT_TRUE(DelayedSingleton<DeviceEventCallback>::GetInstance()->deviceEventCallback_ == callback);
}

/**
* @tc.name: UnRegisterGameDeviceEventCallback_001
* @tc.desc: After unregistration, deviceEventCallback_ is nullptr.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameDeviceClientTest, UnRegisterGameDeviceEventCallback_001, TestSize.Level0)
{
    DelayedSingleton<DeviceEventCallback>::GetInstance()->deviceEventCallback_
        = std::make_shared<GameDeviceEventCallback>();
    GameDeviceClient::UnRegisterGameDeviceEventCallback(ApiTypeEnum::CAPI);
    ASSERT_TRUE(DelayedSingleton<DeviceEventCallback>::GetInstance()->deviceEventCallback_ == nullptr);
}

}
}