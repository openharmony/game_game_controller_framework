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
#include "gamecontroller_client_model.h"
#include <gtest/gtest.h>
#include "refbase.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
namespace {
const int32_t VENDOR = 12;
const int32_t PRODUCT = 13;
}
class GameControllerClientModeTest : public testing::Test {
};

/**
* @tc.name: IdentifiedDeviceInfo_CheckParamValid_001
* @tc.desc: check IdentifiedDeviceInfo is valid
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerClientModeTest, IdentifiedDeviceInfo_CheckParamValid_001, TestSize.Level0)
{
    IdentifiedDeviceInfo deviceInfo;
    deviceInfo.name = std::string(MAX_DEVICE_NAME_LENGTH, 'a');
    deviceInfo.deviceType = DeviceTypeEnum::GAME_PAD;
    ASSERT_TRUE(deviceInfo.CheckParamValid());

    deviceInfo.name = "";
    ASSERT_FALSE(deviceInfo.CheckParamValid());

    deviceInfo.name = std::string(MAX_DEVICE_NAME_LENGTH + 1, 'a');
    ASSERT_FALSE(deviceInfo.CheckParamValid());
}

/**
* @tc.name: DeviceInfo_CheckParamValid_001
* @tc.desc: check deviceInfo is valid
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerClientModeTest, DeviceInfo_CheckParamValid_001, TestSize.Level0)
{
    DeviceInfo deviceInfo;
    deviceInfo.uniq = std::string(MAX_UNIQ_LENGTH, 'a');
    deviceInfo.name = std::string(MAX_DEVICE_NAME_LENGTH, 'c');
    ASSERT_TRUE(deviceInfo.CheckParamValid());

    deviceInfo.uniq = "";
    ASSERT_FALSE(deviceInfo.CheckParamValid());

    deviceInfo.uniq = std::string(MAX_UNIQ_LENGTH + 1, 'a');
    ASSERT_FALSE(deviceInfo.CheckParamValid());

    deviceInfo.uniq = std::string(MAX_UNIQ_LENGTH, 'a');
    deviceInfo.name = "";
    ASSERT_FALSE(deviceInfo.CheckParamValid());

    deviceInfo.name = std::string(MAX_DEVICE_NAME_LENGTH + 1, 'a');
    ASSERT_FALSE(deviceInfo.CheckParamValid());
}

/**
* @tc.name: DeviceInfo_UniqIsEmpty_001
* @tc.desc: check uniq is empty
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerClientModeTest, DeviceInfo_UniqIsEmpty_001, TestSize.Level0)
{
    DeviceInfo deviceInfo;
    ASSERT_TRUE(deviceInfo.UniqIsEmpty());

    deviceInfo.uniq = "null";
    ASSERT_TRUE(deviceInfo.UniqIsEmpty());

    deviceInfo.uniq = "11";
    ASSERT_FALSE(deviceInfo.UniqIsEmpty());
}

/**
* @tc.name: DeviceInfo_GetVidPid_001
* @tc.desc: check vidPid is equal vid + _ + pid
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerClientModeTest, DeviceInfo_GetVidPid_001, TestSize.Level0)
{
    DeviceInfo deviceInfo;
    deviceInfo.vendor = VENDOR;
    deviceInfo.product = PRODUCT;
    ASSERT_EQ("12_13", deviceInfo.GetVidPid());
}
}
}
