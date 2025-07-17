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

#include "device_event_callback.h"

#undef private

#include "device_event_callback_impl.h"
#include <gtest/gtest.h>
#include "refbase.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
class DeviceEventCallbackTest : public testing::Test {
public:
    void SetUp() override;

    void TearDown() override;
};

void DeviceEventCallbackTest::SetUp()
{
    DelayedSingleton<DeviceEventCallback>::GetInstance()->deviceEventCallback_ = nullptr;
}

void DeviceEventCallbackTest::TearDown()
{
    DelayedSingleton<DeviceEventCallback>::GetInstance()->deviceEventCallback_ = nullptr;
}

/**
* @tc.name: GetAllDeviceInfos_001
* @tc.desc: If a callback method exists, the callback is successful.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(DeviceEventCallbackTest, OnDeviceEventCallback_001, TestSize.Level0)
{
    std::shared_ptr<GameDeviceEventCallback> callback = std::make_shared<GameDeviceEventCallback>();
    DelayedSingleton<DeviceEventCallback>::GetInstance()->deviceEventCallback_ = callback;
    DeviceEvent deviceEvent;
    deviceEvent.changeType = 1;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "uniq";
    deviceEvent.deviceInfo = deviceInfo;

    DelayedSingleton<DeviceEventCallback>::GetInstance()->OnDeviceEventCallback(deviceEvent);

    ASSERT_EQ(callback->result_.changeType, 1);
    ASSERT_EQ(callback->result_.deviceInfo.uniq, deviceInfo.uniq);
}

/**
* @tc.name: OnDeviceEventCallback_002
* @tc.desc: When the callback method is canceled, the event is discarded.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(DeviceEventCallbackTest, OnDeviceEventCallback_002, TestSize.Level0)
{
    std::shared_ptr<GameDeviceEventCallback> callback = std::make_shared<GameDeviceEventCallback>();
    DelayedSingleton<DeviceEventCallback>::GetInstance()->deviceEventCallback_ = callback;
    DelayedSingleton<DeviceEventCallback>::GetInstance()->UnRegisterGameDeviceEventCallback(ApiTypeEnum::CAPI);
    DeviceEvent deviceEvent;
    deviceEvent.changeType = 1;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "uniq";
    deviceEvent.deviceInfo = deviceInfo;

    DelayedSingleton<DeviceEventCallback>::GetInstance()->OnDeviceEventCallback(deviceEvent);

    ASSERT_NE(callback->result_.changeType, 1);
    ASSERT_NE(callback->result_.deviceInfo.uniq, deviceInfo.uniq);
}
}
}