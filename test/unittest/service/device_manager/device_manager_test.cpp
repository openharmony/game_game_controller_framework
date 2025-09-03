/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/hwext/gtest-ext.h"
#include "gtest/hwext/gtest-tag.h"
#include "gmock/gmock-actions.h"
#include "gmock/gmock-spec-builders.h"
#include "gamecontroller_errors.h"

#define private public

#include "device_manager.h"

#undef private

#include <gtest/gtest.h>
#include <string>
#include "refbase.h"

using namespace testing::ext;
using namespace std;
using json = nlohmann::json;

namespace OHOS {
namespace GameController {
namespace {
const int32_t MAX_DEVICE_NUMBER = 10000;
}
class DeviceManagerTest : public testing::Test {
public:
    void SetUp();

    void TearDown();

    void LoadTestCache();
};

void DeviceManagerTest::SetUp()
{
    DelayedSingleton<GameController::DeviceManager>::GetInstance()->identifiedDevices_.clear();
}

void DeviceManagerTest::TearDown()
{
}

void DeviceManagerTest::LoadTestCache()
{
    std::string content = "{\"GamePad\":[{\"NamePrefix\":\"BEITONG_A1N2\",\"Product\":33382,\"Vendor\":9354}]}";
    json js = json::parse(content);
    DelayedSingleton<GameController::DeviceManager>::GetInstance()->LoadFromJson(js);
    ASSERT_FALSE(DelayedSingleton<GameController::DeviceManager>::GetInstance()->identifiedDevices_.empty());
}

/**
* @tc.name: IdentifyDeviceInfo_IsMatched_001
* @tc.desc: device is matched by product and version
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, IdentifyDeviceInfo_IsMatched_001, TestSize.Level0)
{
    std::string content = "{\"NamePrefix\":\"BEITONG_A1N2-125P4002552\",\"Product\":33382,"
                          "\"Vendor\":9354}";
    json js = json::parse(content);
    IdentifyDeviceInfo authed(js);
    EXPECT_EQ("BEITONG_A1N2-125P4002552", authed.namePrefix);
    EXPECT_EQ(33382, authed.productId);
    EXPECT_EQ(9354, authed.vendor);

    DeviceInfo device;
    device.product = 33382;
    device.vendor = 9354;
    device.name = "ZZZ";
    EXPECT_TRUE(authed.IsMatched(device));
}

/**
* @tc.name: IdentifyDeviceInfo_IsMatched_002
* @tc.desc: device is matched by name
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, IdentifyDeviceInfo_IsMatched_002, TestSize.Level0)
{
    std::string content = "{\"NamePrefix\":\"BEITONG_A1N2-125P4002552\",\"Product\":33382,\"Vendor\":9354}";
    json js = json::parse(content);
    IdentifyDeviceInfo authed(js);

    DeviceInfo device;
    device.product = -1;
    device.vendor = -1;

    // Only the name is fuzzy match.
    device.name = "BEITONG_A1N2-125P4002552-98089";
    EXPECT_TRUE(authed.IsMatched(device));
}

/**
* @tc.name: IdentifyDeviceInfo_IsMatched_003
* @tc.desc: device is not matched
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, IdentifyDeviceInfo_IsMatched_003, TestSize.Level0)
{
    std::string content = "{\"NamePrefix\":\"BEITONG_A1N2-125P4002552\",\"Product\":33382,\"Vendor\":9354}";
    json js = json::parse(content);
    IdentifyDeviceInfo authed(js);

    DeviceInfo device;
    device.product = -1;
    device.vendor = -1;

    // Only the name is fuzzy match.
    device.name = "BEITONG_A1N2-xxxx";
    EXPECT_FALSE(authed.IsMatched(device));
}

/**
* @tc.name: DeviceIdentify_001
* @tc.desc: identify device when input devices is empty
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, DeviceIdentify_001, TestSize.Level0)
{
    LoadTestCache();
    std::vector<DeviceInfo> devices;
    std::vector<DeviceInfo> results;
    int32_t ret = DelayedSingleton<GameController::DeviceManager>::GetInstance()->DeviceIdentify(devices, results);
    EXPECT_EQ(0, ret);
    EXPECT_TRUE(results.empty());
}

/**
* @tc.name: DeviceIdentify_002
* @tc.desc: identify device when identified devices is empty
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, DeviceIdentify_002, TestSize.Level0)
{
    std::vector<DeviceInfo> devices;
    DeviceInfo device;
    device.product = 33382;
    device.vendor = 9354;
    device.name = "BEITONG_A1N2-125P4002552";
    device.deviceType = DeviceTypeEnum::GAME_PAD;
    devices.push_back(device);

    ASSERT_FALSE(devices.empty());
    std::vector<DeviceInfo> results;
    int32_t ret = DelayedSingleton<GameController::DeviceManager>::GetInstance()->DeviceIdentify(devices, results);
    EXPECT_EQ(0, ret);
    ASSERT_FALSE(results.empty());
    EXPECT_EQ(DeviceTypeEnum::UNKNOWN, results[0].deviceType);
}

/**
* @tc.name: DeviceIdentify_003
* @tc.desc: identify device but device product not matched
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, DeviceIdentify_003, TestSize.Level0)
{
    LoadTestCache();
    std::vector<DeviceInfo> devices;
    DeviceInfo device;
    device.product = -1;
    device.vendor = 9354;
    device.name = "ZZZ";
    device.deviceType = DeviceTypeEnum::GAME_PAD;
    devices.push_back(device);

    ASSERT_FALSE(devices.empty());
    std::vector<DeviceInfo> results;
    int32_t ret = DelayedSingleton<GameController::DeviceManager>::GetInstance()->DeviceIdentify(devices, results);
    EXPECT_EQ(0, ret);
    ASSERT_FALSE(results.empty());
    EXPECT_EQ(DeviceTypeEnum::UNKNOWN, results[0].deviceType);
}

/**
* @tc.name: DeviceIdentify_004
* @tc.desc: auth device but device vendor not matched
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, DeviceIdentify_004, TestSize.Level0)
{
    LoadTestCache();
    std::vector<DeviceInfo> devices;
    DeviceInfo device;
    device.product = 33382;
    device.vendor = -1;
    device.name = "ZZZ";
    device.deviceType = DeviceTypeEnum::GAME_PAD;
    devices.push_back(device);

    ASSERT_FALSE(devices.empty());
    std::vector<DeviceInfo> results;
    int32_t ret = DelayedSingleton<GameController::DeviceManager>::GetInstance()->DeviceIdentify(devices, results);
    EXPECT_EQ(0, ret);
    ASSERT_FALSE(results.empty());
    EXPECT_EQ(DeviceTypeEnum::UNKNOWN, results[0].deviceType);
}

/**
* @tc.name: DeviceIdentify_005
* @tc.desc: identify device and device name not matched, product and vendor matched
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, DeviceIdentify_005, TestSize.Level0)
{
    LoadTestCache();
    std::vector<DeviceInfo> devices;
    DeviceInfo device;
    device.product = 33382;
    device.vendor = 9354;
    device.name = "ZZZZZ";
    devices.push_back(device);

    ASSERT_FALSE(devices.empty());
    std::vector<DeviceInfo> results;
    int32_t ret = DelayedSingleton<GameController::DeviceManager>::GetInstance()->DeviceIdentify(devices, results);
    EXPECT_EQ(0, ret);
    ASSERT_FALSE(results.empty());
    EXPECT_EQ(DeviceTypeEnum::GAME_PAD, results[0].deviceType);
}

/**
* @tc.name: DeviceIdentify_006
* @tc.desc: identify device
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, DeviceIdentify_006, TestSize.Level0)
{
    LoadTestCache();
    std::vector<DeviceInfo> devices;
    DeviceInfo device;
    device.product = 33382;
    device.vendor = 9354;
    device.name = "BEITONG_A1N2-125P4002552";
    devices.push_back(device);

    ASSERT_FALSE(devices.empty());
    std::vector<DeviceInfo> results;
    int32_t ret = DelayedSingleton<GameController::DeviceManager>::GetInstance()->DeviceIdentify(devices, results);
    EXPECT_EQ(0, ret);
    ASSERT_FALSE(results.empty());
    EXPECT_EQ(DeviceTypeEnum::GAME_PAD, results[0].deviceType);
    EXPECT_EQ("BEITONG_A1N2-125P4002552", results[0].name);
}

/**
* @tc.name: LoadFromJson_001
* @tc.desc: parse json content with exact GamePad
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, LoadFromJson_001, TestSize.Level0)
{
    std::string content = "{\"GamePad\":[{\"NamePrefix\":\"BEITONG_A1N2-125P4002552\",\"Product\":33382,"
                          "\"Vendor\":9354}]}";
    json js = json::parse(content);
    DelayedSingleton<GameController::DeviceManager>::GetInstance()->LoadFromJson(js);
    EXPECT_EQ(1, DelayedSingleton<GameController::DeviceManager>::GetInstance()->identifiedDevices_.size());
}

/**
* @tc.name: LoadFromJson_002
* @tc.desc: parse json content with GamePad and UnKnow device
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, LoadFromJson_002, TestSize.Level0)
{
    std::string content = "{\"GamePad\":[{\"NamePrefix\":\"BEITONG_A1N2-125P4002552\",\"Product\":33382,"
                          "\"Vendor\":9354}],\"Mouse\":[{\"NamePrefix\":\"M585/M590 Mouse\","
                          "\"Product\":45083,\"Vendor\":1133}]}";
    json js = json::parse(content);
    DelayedSingleton<GameController::DeviceManager>::GetInstance()->LoadFromJson(js);
    EXPECT_EQ(2, DelayedSingleton<GameController::DeviceManager>::GetInstance()->identifiedDevices_.size());
}

/**
* @tc.name: LoadFromJson_003
* @tc.desc: parse json content with empty config
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, LoadFromJson_003, TestSize.Level0)
{
    std::string content = "{}";
    json js = json::parse(content);
    DelayedSingleton<GameController::DeviceManager>::GetInstance()->LoadFromJson(js);
    EXPECT_EQ(0, DelayedSingleton<GameController::DeviceManager>::GetInstance()->identifiedDevices_.size());
}

static bool IsSameIdentifyDeviceInfo(const IdentifiedDeviceInfo &info, const IdentifyDeviceInfo &target)
{
    return info.product == target.productId && info.vendor == target.vendor && info.name == target.namePrefix;
}

/**
* @tc.name: SyncIdentifiedDeviceInfos_001
* @tc.desc: SyncIdentifiedDeviceInfos success
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, SyncIdentifiedDeviceInfos_001, TestSize.Level0)
{
    std::vector<IdentifiedDeviceInfo> list;
    IdentifiedDeviceInfo identifyDeviceInfo1;
    identifyDeviceInfo1.deviceType = DeviceTypeEnum::GAME_PAD;
    identifyDeviceInfo1.vendor = 93541;
    identifyDeviceInfo1.product = 333821;
    identifyDeviceInfo1.name = "BEITONG_A1N22";
    list.push_back(identifyDeviceInfo1);
    IdentifiedDeviceInfo identifyDeviceInfo2;
    identifyDeviceInfo2.deviceType = DeviceTypeEnum::GAME_PAD;
    identifyDeviceInfo2.vendor = 205741;
    identifyDeviceInfo2.product = 205741;
    identifyDeviceInfo2.name = "JZ-V4 BFM2";
    list.push_back(identifyDeviceInfo2);
    IdentifiedDeviceInfo identifyDeviceInfo3;
    identifyDeviceInfo3.deviceType = DeviceTypeEnum::UNKNOWN;
    identifyDeviceInfo3.vendor = 205743;
    identifyDeviceInfo3.product = 205743;
    identifyDeviceInfo3.name = "JZ-V4 BFM3";
    list.push_back(identifyDeviceInfo3);
    int32_t result = DelayedSingleton<GameController::DeviceManager>::GetInstance()->SyncIdentifiedDeviceInfos(list);
    EXPECT_EQ(result, GAME_CONTROLLER_SUCCESS);
    EXPECT_EQ(2, DelayedSingleton<GameController::DeviceManager>::GetInstance()->identifiedDevices_.size());
    DelayedSingleton<GameController::DeviceManager>::GetInstance()->identifiedDevices_.clear();
    DelayedSingleton<GameController::DeviceManager>::GetInstance()->LoadDeviceCacheFile();
    EXPECT_EQ(2, DelayedSingleton<GameController::DeviceManager>::GetInstance()->identifiedDevices_.size());
    for (auto identifyDeviceInfo:
        DelayedSingleton<GameController::DeviceManager>::GetInstance()->identifiedDevices_) {
        EXPECT_TRUE(IsSameIdentifyDeviceInfo(identifyDeviceInfo1, identifyDeviceInfo) ||
                        IsSameIdentifyDeviceInfo(identifyDeviceInfo2, identifyDeviceInfo));
    }
}

/**
* @tc.name: SyncIdentifiedDeviceInfos_002
* @tc.desc: over max device numbers
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, SyncIdentifiedDeviceInfos_002, TestSize.Level0)
{
    std::vector<IdentifiedDeviceInfo> list;
    IdentifiedDeviceInfo identifyDeviceInfo1;
    identifyDeviceInfo1.deviceType = DeviceTypeEnum::GAME_PAD;
    identifyDeviceInfo1.vendor = 93541;
    identifyDeviceInfo1.product = 333821;
    identifyDeviceInfo1.name = "BEITONG_A1N22";
    for (int32_t idx = 0; idx <= MAX_DEVICE_NUMBER; idx++) {
        list.push_back(identifyDeviceInfo1);
    }
    int32_t result = DelayedSingleton<GameController::DeviceManager>::GetInstance()->SyncIdentifiedDeviceInfos(list);
    EXPECT_EQ(result, GAME_ERR_ARRAY_MAXSIZE);

    list.clear();
    for (int32_t idx = 0; idx < MAX_DEVICE_NUMBER; idx++) {
        list.push_back(identifyDeviceInfo1);
    }
    result = DelayedSingleton<GameController::DeviceManager>::GetInstance()->SyncIdentifiedDeviceInfos(list);
    EXPECT_EQ(result, GAME_CONTROLLER_SUCCESS);
}

/**
* @tc.name: SyncIdentifiedDeviceInfos_003
* @tc.desc: It's empty devices
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, SyncIdentifiedDeviceInfos_003, TestSize.Level0)
{
    std::vector<IdentifiedDeviceInfo> list;
    int32_t result = DelayedSingleton<GameController::DeviceManager>::GetInstance()->SyncIdentifiedDeviceInfos(list);
    EXPECT_EQ(result, GAME_CONTROLLER_SUCCESS);
    EXPECT_EQ(0, DelayedSingleton<GameController::DeviceManager>::GetInstance()->identifiedDevices_.size());
    DelayedSingleton<GameController::DeviceManager>::GetInstance()->identifiedDevices_.clear();
    DelayedSingleton<GameController::DeviceManager>::GetInstance()->LoadDeviceCacheFile();
    EXPECT_EQ(0, DelayedSingleton<GameController::DeviceManager>::GetInstance()->identifiedDevices_.size());
}

/**
* @tc.name: SyncIdentifiedDeviceInfos_004
* @tc.desc: Parameter (name) is invalid
* @tc.type: FUNC
* @tc.require: AR000JTBO4
*/
HWTEST_F(DeviceManagerTest, SyncIdentifiedDeviceInfos_004, TestSize.Level0)
{
    std::vector<IdentifiedDeviceInfo> list;
    IdentifiedDeviceInfo identifyDeviceInfo1;
    identifyDeviceInfo1.vendor = 93541;
    identifyDeviceInfo1.product = 333821;
    identifyDeviceInfo1.deviceType = DeviceTypeEnum::GAME_PAD;
    identifyDeviceInfo1.name = "";
    list.push_back(identifyDeviceInfo1);
    int32_t result = DelayedSingleton<GameController::DeviceManager>::GetInstance()->SyncIdentifiedDeviceInfos(list);
    EXPECT_EQ(result, GAME_ERR_ARGUMENT_INVALID);
}
} // namespace GameController
} // namespace OHOS
