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

#include "multi_modal_input_mgt_service.h"
#include "device_info_service_mock.h"
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
namespace {
const int32_t CACHE_DEVICE_ID = 100;
}

class MultiModalInputMgtServiceTest : public testing::Test {
public:
    void SetUp() override;

    void TearDown() override;

    static InputDeviceInfo CreateInputDeviceInfo(int32_t deviceId)
    {
        std::string temp = std::to_string(deviceId);
        InputDeviceInfo inputDeviceInfo;
        inputDeviceInfo.uniq = "11:22:33:44:" + temp;
        inputDeviceInfo.id = deviceId;
        inputDeviceInfo.product = deviceId;
        inputDeviceInfo.name = "name" + temp;
        inputDeviceInfo.phys = "phys" + temp;
        inputDeviceInfo.vendor = deviceId;
        inputDeviceInfo.version = deviceId;
        return inputDeviceInfo;
    }

public:
    std::shared_ptr<DeviceInfoServiceMock> deviceInfoServiceMock_;
    std::shared_ptr<GameControllerServerClientMock> gameControllerServerClientMock_;
};

static void ClearDeviceCache()
{
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.clear();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.clear();
}

void MultiModalInputMgtServiceTest::SetUp()
{
    gameControllerServerClientMock_ = std::make_shared<GameControllerServerClientMock>();
    GameControllerServerClient::instance_ = gameControllerServerClientMock_;
    deviceInfoServiceMock_ = std::make_shared<DeviceInfoServiceMock>();
    DeviceInfoService::instance_ = deviceInfoServiceMock_;
    ClearDeviceCache();
}

void MultiModalInputMgtServiceTest::TearDown()
{
    gameControllerServerClientMock_.reset();
    DeviceInfoService::instance_ = nullptr;
    deviceInfoServiceMock_.reset();
    ClearDeviceCache();
}

static void InitDeviceCache()
{
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "11:22:66:77:XXXX";
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_[deviceInfo.uniq] = deviceInfo;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[CACHE_DEVICE_ID] = deviceInfo.uniq;
}

/**
* @tc.name: GetAllDeviceInfos_001
* @tc.desc: The same uniq is aggregated. The name is the name of the last device.
 * The device that does not exist in the local cache is automatically cleared.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(MultiModalInputMgtServiceTest, GetAllDeviceInfos_001, TestSize.Level0)
{
    // inputDeviceInfo1 and inputDeviceInfo2 have same uniq
    InitDeviceCache();
    std::pair<int32_t, std::vector<InputDeviceInfo>> pair;
    pair.first = 0;
    std::vector<InputDeviceInfo> vector;
    InputDeviceInfo inputDeviceInfo1 = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(1);
    InputDeviceInfo inputDeviceInfo2 = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(2);
    inputDeviceInfo2.uniq = inputDeviceInfo1.uniq;
    vector.push_back(inputDeviceInfo1);
    vector.push_back(inputDeviceInfo2);
    InputDeviceInfo inputDeviceInfo3 = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(3);
    vector.push_back(inputDeviceInfo3);
    pair.second = vector;

    EXPECT_CALL(*(deviceInfoServiceMock_.get()), GetAllDeviceInfos()).WillOnce(Return(pair));
    EXPECT_CALL(*(gameControllerServerClientMock_.get()), IdentifyDevice(testing::_, testing::_)).WillOnce(Return(1));
    std::pair<int32_t, std::vector<DeviceInfo>> result = DelayedSingleton<MultiModalInputMgtService>::GetInstance()
        ->GetAllDeviceInfos();

    ASSERT_EQ(pair.first, result.first);
    ASSERT_EQ(2, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.size());
    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.count(
        inputDeviceInfo2.uniq));
    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.count(
        inputDeviceInfo3.uniq));
    ASSERT_EQ(3, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
    ASSERT_EQ(inputDeviceInfo1.uniq,
              DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[inputDeviceInfo1.id]);
    ASSERT_EQ(inputDeviceInfo1.uniq,
              DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[inputDeviceInfo2.id]);
    ASSERT_EQ(inputDeviceInfo3.uniq,
              DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[inputDeviceInfo3.id]);
    ASSERT_EQ(2, result.second.size());
    for (DeviceInfo deviceInfo: result.second) {
        if (deviceInfo.uniq == inputDeviceInfo1.uniq) {
            ASSERT_EQ(inputDeviceInfo2.name, deviceInfo.name);
        } else {
            ASSERT_EQ(inputDeviceInfo3.name, deviceInfo.name);
        }
    }
}

/**
* @tc.name: GetAllDeviceInfos_002
* @tc.desc: If the device information queried from DeviceInfoService::GetAllDeviceInfos is empty,
 * the local cache is cleared.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(MultiModalInputMgtServiceTest, GetAllDeviceInfos_002, TestSize.Level0)
{
    InitDeviceCache();
    std::pair<int32_t, std::vector<InputDeviceInfo>> pair;
    pair.first = 0;

    EXPECT_CALL(*(deviceInfoServiceMock_.get()), GetAllDeviceInfos()).WillOnce(Return(pair));

    std::pair<int32_t, std::vector<DeviceInfo>> result = DelayedSingleton<MultiModalInputMgtService>::GetInstance()
        ->GetAllDeviceInfos();

    ASSERT_EQ(pair.first, result.first);
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.size());
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
}

/**
* @tc.name: GetAllDeviceInfos_003
* @tc.desc: Failed to query device information from DeviceInfoService::GetAllDeviceInfos.
 * The local cache does not need to be cleared.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(MultiModalInputMgtServiceTest, GetAllDeviceInfos_003, TestSize.Level0)
{
    InitDeviceCache();
    std::pair<int32_t, std::vector<InputDeviceInfo>> pair;
    pair.first = 12;

    EXPECT_CALL(*(deviceInfoServiceMock_.get()), GetAllDeviceInfos()).WillOnce(Return(pair));

    std::pair<int32_t, std::vector<DeviceInfo>> result = DelayedSingleton<MultiModalInputMgtService>::GetInstance()
        ->GetAllDeviceInfos();
    ASSERT_EQ(pair.first, result.first);

    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.size());
    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.count(CACHE_DEVICE_ID));
}
/**
* @tc.name: HandleInputDeviceInfo_001
* @tc.desc: Invoke MultiModalInputMgtService::HandleInputDeviceInfo to perform aggregation.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(MultiModalInputMgtServiceTest, HandleInputDeviceInfo_001, TestSize.Level0)
{
    std::unordered_map<std::string, DeviceInfo> deviceInfoByUniqMap;
    InputDeviceInfo inputDeviceInfo1 = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(1);
    inputDeviceInfo1.sourceTypeSet.insert(InputSourceTypeEnum::KEYBOARD);
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->HandleInputDeviceInfo(deviceInfoByUniqMap,
                                                                                      inputDeviceInfo1);
    DeviceInfo deviceInfo = deviceInfoByUniqMap[inputDeviceInfo1.uniq];
    ASSERT_EQ(inputDeviceInfo1.name, deviceInfo.name);
    ASSERT_EQ(inputDeviceInfo1.uniq, deviceInfo.uniq);
    ASSERT_EQ(inputDeviceInfo1.phys, deviceInfo.phys);
    ASSERT_EQ(inputDeviceInfo1.product, deviceInfo.product);
    ASSERT_EQ(inputDeviceInfo1.version, deviceInfo.version);
    ASSERT_EQ(inputDeviceInfo1.vendor, deviceInfo.vendor);
    ASSERT_EQ(1, deviceInfo.ids.size());
    ASSERT_EQ(1, deviceInfo.ids.count(inputDeviceInfo1.id));
    ASSERT_EQ(1, deviceInfo.names.size());
    ASSERT_EQ(1, deviceInfo.names.count(inputDeviceInfo1.name));
    ASSERT_EQ(1, deviceInfo.sourceTypeSet.size());
    ASSERT_EQ(1, deviceInfo.sourceTypeSet.count(InputSourceTypeEnum::KEYBOARD));
    InputDeviceInfo inputDeviceInfo2 = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(2);
    inputDeviceInfo2.sourceTypeSet.insert(InputSourceTypeEnum::JOYSTICK);
    inputDeviceInfo2.uniq = inputDeviceInfo1.uniq;

    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->HandleInputDeviceInfo(deviceInfoByUniqMap,
                                                                                      inputDeviceInfo2);

    ASSERT_EQ(1, deviceInfoByUniqMap.size());
    deviceInfo = deviceInfoByUniqMap[inputDeviceInfo1.uniq];
    ASSERT_EQ(inputDeviceInfo2.name, deviceInfo.name);
    ASSERT_EQ(inputDeviceInfo1.uniq, deviceInfo.uniq);
    ASSERT_EQ(inputDeviceInfo1.phys, deviceInfo.phys);
    ASSERT_EQ(inputDeviceInfo1.product, deviceInfo.product);
    ASSERT_EQ(inputDeviceInfo1.version, deviceInfo.version);
    ASSERT_EQ(inputDeviceInfo1.vendor, deviceInfo.vendor);
    ASSERT_EQ(2, deviceInfo.ids.size());
    ASSERT_EQ(1, deviceInfo.ids.count(inputDeviceInfo1.id));
    ASSERT_EQ(1, deviceInfo.ids.count(inputDeviceInfo2.id));
    ASSERT_EQ(2, deviceInfo.names.size());
    ASSERT_EQ(1, deviceInfo.names.count(inputDeviceInfo1.name));
    ASSERT_EQ(1, deviceInfo.names.count(inputDeviceInfo2.name));
    ASSERT_EQ(2, deviceInfo.sourceTypeSet.size());
    ASSERT_EQ(1, deviceInfo.sourceTypeSet.count(InputSourceTypeEnum::JOYSTICK));
    ASSERT_EQ(1, deviceInfo.sourceTypeSet.count(InputSourceTypeEnum::KEYBOARD));
}

/**
* @tc.name: GetDeviceInfo_001
* @tc.desc: When DeviceInfo is queried by deviceId, deviceIdUniqMap_ does not contain deviceId.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(MultiModalInputMgtServiceTest, GetDeviceInfo_001, TestSize.Level0)
{
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.clear();
    int32_t deviceId = 1;

    DeviceInfo deviceInfo = DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetDeviceInfo(deviceId);

    ASSERT_EQ("", deviceInfo.uniq);
}

/**
* @tc.name: GetDeviceInfo_002
* @tc.desc: Query DeviceInfo by deviceId. The device information is correctly obtained.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(MultiModalInputMgtServiceTest, GetDeviceInfo_002, TestSize.Level0)
{
    ClearDeviceCache();
    DeviceInfo outDeviceInfo;
    outDeviceInfo.uniq = "11212";
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_[outDeviceInfo.uniq] =
        outDeviceInfo;
    int32_t deviceId = 1;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[deviceId] = outDeviceInfo.uniq;

    DeviceInfo deviceInfo = DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetDeviceInfo(deviceId);

    ASSERT_EQ(outDeviceInfo.uniq, deviceInfo.uniq);
}

/**
* @tc.name: GetDeviceInfo_003
* @tc.desc: When DeviceInfo is queried by deviceId, deviceInfoByUniqMap_ does not
 * have the corresponding device information, the record of deviceIdUniqMap_ is deleted.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(MultiModalInputMgtServiceTest, GetDeviceInfo_003, TestSize.Level0)
{
    ClearDeviceCache();
    DeviceInfo outDeviceInfo;
    outDeviceInfo.uniq = "11212";
    int32_t deviceId = 1;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[deviceId] = outDeviceInfo.uniq;

    DeviceInfo deviceInfo = DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetDeviceInfo(deviceId);

    ASSERT_EQ("", deviceInfo.uniq);
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
}

/**
* @tc.name: HandleDeviceChangeEvent_001
* @tc.desc: When a device goes online, the local cache is cleared after the device goes offline.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(MultiModalInputMgtServiceTest, HandleDeviceChangeEvent_001, TestSize.Level0)
{
    InitDeviceCache();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_ = false;
    DeviceChangeEvent event;
    event.deviceChangeType = DeviceChangeType::REMOVE;
    event.deviceId = -1;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event);

    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->HandleDeviceChangeEvent();

    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.size());
    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
    ASSERT_TRUE(DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_);
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.size());
}

/**
* @tc.name: HandleDeviceChangeEvent_002
* @tc.desc: When a device is offline, the local cache is not cleared after the device is offline.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(MultiModalInputMgtServiceTest, HandleDeviceChangeEvent_002, TestSize.Level0)
{
    InitDeviceCache();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_ = false;
    DeviceChangeEvent event;
    event.deviceChangeType = DeviceChangeType::REMOVE;
    event.deviceId = CACHE_DEVICE_ID;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event);

    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->HandleDeviceChangeEvent();

    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.size());
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
    ASSERT_TRUE(DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_);
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.size());
}

/**
 * @tc.name: HandleDeviceChangeEvent_003
 * @tc.desc: Receives two online events from the same device, aggregates them, and caches them locally.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MultiModalInputMgtServiceTest, HandleDeviceChangeEvent_003, TestSize.Level0)
{
    // Construct two online events of the same device.
    ClearDeviceCache();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_ = false;
    DeviceChangeEvent event1;
    event1.deviceChangeType = DeviceChangeType::ADD;
    event1.deviceId = 1;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event1);
    DeviceChangeEvent event2;
    event2.deviceChangeType = DeviceChangeType::ADD;
    event2.deviceId = 2;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event2);

    // Query result of the first device
    std::pair<int32_t, InputDeviceInfo> pair1;
    pair1.first = 0;
    InputDeviceInfo inputDeviceInfo1 = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(event1.deviceId);
    inputDeviceInfo1.sourceTypeSet.insert(InputSourceTypeEnum::KEYBOARD);
    pair1.second = inputDeviceInfo1;
    EXPECT_CALL(*(deviceInfoServiceMock_.get()), GetInputDeviceInfo(event1.deviceId)).WillOnce(Return(pair1));

    // Query result of the second device
    std::pair<int32_t, InputDeviceInfo> pair2;
    pair2.first = 0;
    InputDeviceInfo inputDeviceInfo2 = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(event2.deviceId);
    inputDeviceInfo2.sourceTypeSet.insert(InputSourceTypeEnum::JOYSTICK);
    inputDeviceInfo2.uniq = inputDeviceInfo1.uniq;
    pair2.second = inputDeviceInfo2;
    EXPECT_CALL(*(deviceInfoServiceMock_.get()), GetInputDeviceInfo(event2.deviceId)).WillOnce(Return(pair2));
    EXPECT_CALL(*(gameControllerServerClientMock_.get()), IdentifyDevice(testing::_, testing::_)).WillOnce(Return(1));

    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->HandleDeviceChangeEvent();

    ASSERT_TRUE(DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_);
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.size());
    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.size());
    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.count(
        inputDeviceInfo2.uniq));
    ASSERT_EQ(2, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
    ASSERT_EQ(inputDeviceInfo1.uniq,
              DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[inputDeviceInfo1.id]);
    ASSERT_EQ(inputDeviceInfo1.uniq,
              DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[inputDeviceInfo2.id]);
    DeviceInfo deviceInfo = DelayedSingleton<MultiModalInputMgtService>::GetInstance()
        ->deviceInfoByUniqMap_[inputDeviceInfo1.uniq];
    ASSERT_EQ(inputDeviceInfo1.uniq, deviceInfo.uniq);
}

/**
 * @tc.name: HandleDeviceChangeEvent_004
 * @tc.desc: If the device online information is received and the device information fails to be queried,
 * the event is discarded.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MultiModalInputMgtServiceTest, HandleDeviceChangeEvent_004, TestSize.Level0)
{
    ClearDeviceCache();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_ = false;
    DeviceChangeEvent event1;
    event1.deviceChangeType = DeviceChangeType::ADD;
    event1.deviceId = 1;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event1);
    std::pair<int32_t, InputDeviceInfo> pair1;
    pair1.first = 1212;
    EXPECT_CALL(*(deviceInfoServiceMock_.get()), GetInputDeviceInfo(event1.deviceId)).WillOnce(Return(pair1));

    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->HandleDeviceChangeEvent();

    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.size());
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
    ASSERT_TRUE(DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_);
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.size());
}

/**
 * @tc.name: HandleDeviceChangeEvent_005
 * @tc.desc: A device's online and offline messages are received at the same time,
 * and the event sequence is online > offline. Finally, the local cache is empty.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MultiModalInputMgtServiceTest, HandleDeviceChangeEvent_005, TestSize.Level0)
{
    ClearDeviceCache();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_ = false;
    DeviceChangeEvent event1;
    event1.deviceChangeType = DeviceChangeType::ADD;
    event1.deviceId = 1;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event1);
    DeviceChangeEvent event2;
    event2.deviceChangeType = DeviceChangeType::REMOVE;
    event2.deviceId = event1.deviceId;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event2);
    ASSERT_EQ(2, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.size());

    // Query result of the first device
    std::pair<int32_t, InputDeviceInfo> pair1;
    pair1.first = 0;
    InputDeviceInfo inputDeviceInfo1 = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(event1.deviceId);
    inputDeviceInfo1.sourceTypeSet.insert(InputSourceTypeEnum::KEYBOARD);
    pair1.second = inputDeviceInfo1;
    EXPECT_CALL(*(deviceInfoServiceMock_.get()), GetInputDeviceInfo(event1.deviceId)).WillOnce(Return(pair1));

    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->HandleDeviceChangeEvent();

    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.size());
    ASSERT_TRUE(DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_);
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.size());
}

/**
 * @tc.name: HandleDeviceChangeEvent_006
 * @tc.desc: The online and offline events of a device are received at the same time.
 * The event sequence is offline and then online. Finally, the local cache is generated.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MultiModalInputMgtServiceTest, HandleDeviceChangeEvent_006, TestSize.Level0)
{
    ClearDeviceCache();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_ = false;
    DeviceChangeEvent event1;
    event1.deviceChangeType = DeviceChangeType::REMOVE;
    event1.deviceId = 1;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event1);
    DeviceChangeEvent event2;
    event2.deviceChangeType = DeviceChangeType::ADD;
    event2.deviceId = 1;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event2);

    // Query result of the first device
    std::pair<int32_t, InputDeviceInfo> pair1;
    pair1.first = 0;
    InputDeviceInfo inputDeviceInfo1 = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(event1.deviceId);
    inputDeviceInfo1.sourceTypeSet.insert(InputSourceTypeEnum::KEYBOARD);
    pair1.second = inputDeviceInfo1;
    EXPECT_CALL(*(deviceInfoServiceMock_.get()), GetInputDeviceInfo(event1.deviceId)).WillOnce(Return(pair1));
    EXPECT_CALL(*(gameControllerServerClientMock_.get()), IdentifyDevice(testing::_, testing::_)).WillOnce(Return(1));

    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->HandleDeviceChangeEvent();

    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.size());

    ASSERT_EQ(inputDeviceInfo1.uniq,
              DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[inputDeviceInfo1.id]);
    DeviceInfo deviceInfo = DelayedSingleton<MultiModalInputMgtService>::GetInstance()
        ->deviceInfoByUniqMap_[inputDeviceInfo1.uniq];
    ASSERT_EQ(inputDeviceInfo1.uniq, deviceInfo.uniq);
    ASSERT_EQ(DeviceTypeEnum::UNKNOWN, deviceInfo.deviceType);
    ASSERT_TRUE(DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_);
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.size());
}

/**
 * @tc.name: HandleDeviceChangeEvent_007
 * @tc.desc: Receives virtual device event. if it's external device's virtual device event,
 * Construct uniq using vendor and product
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MultiModalInputMgtServiceTest, HandleDeviceChangeEvent_007, TestSize.Level0)
{
    // Construct two online events of the same device.
    ClearDeviceCache();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_ = false;
    DeviceChangeEvent event1;
    event1.deviceChangeType = DeviceChangeType::ADD;
    event1.deviceId = 1;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event1);
    DeviceChangeEvent event2;
    event2.deviceChangeType = DeviceChangeType::ADD;
    event2.deviceId = 2;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event2);

    // Query result of the first device
    std::pair<int32_t, InputDeviceInfo> pair1;
    pair1.first = 0;
    InputDeviceInfo inputDeviceInfo1 = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(event1.deviceId);
    inputDeviceInfo1.sourceTypeSet.insert(InputSourceTypeEnum::KEYBOARD);
    pair1.second = inputDeviceInfo1;
    EXPECT_CALL(*(deviceInfoServiceMock_.get()), GetInputDeviceInfo(event1.deviceId)).WillOnce(Return(pair1));

    /*
     * Query result of the second device. inputDeviceInfo2's product and vendor are same where inputDeviceInfo1.
     * but uniq is empty.
     */
    std::pair<int32_t, InputDeviceInfo> pair2;
    pair2.first = 0;
    InputDeviceInfo inputDeviceInfo2 = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(event2.deviceId);
    inputDeviceInfo2.sourceTypeSet.insert(InputSourceTypeEnum::JOYSTICK);
    inputDeviceInfo2.uniq = "";
    inputDeviceInfo2.product = inputDeviceInfo1.product;
    inputDeviceInfo2.vendor = inputDeviceInfo1.vendor;
    pair2.second = inputDeviceInfo2;
    EXPECT_CALL(*(deviceInfoServiceMock_.get()), GetInputDeviceInfo(event2.deviceId)).WillOnce(Return(pair2));
    EXPECT_CALL(*(gameControllerServerClientMock_.get()), IdentifyDevice(testing::_, testing::_)).WillOnce(Return(1));
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->HandleDeviceChangeEvent();

    ASSERT_TRUE(DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_);
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.size());
    ASSERT_EQ(2, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.size());
    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.count(
        inputDeviceInfo1.uniq));
    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.count(
        inputDeviceInfo1.uniq));
    ASSERT_EQ(2, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
    ASSERT_EQ(inputDeviceInfo1.uniq,
              DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[inputDeviceInfo1.id]);
    std::string uniq = std::to_string(inputDeviceInfo2.vendor) + "_" + std::to_string(inputDeviceInfo2.product);
    ASSERT_EQ(uniq, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[inputDeviceInfo2.id]);
}

/**
 * @tc.name: HandleDeviceChangeEvent_008
 * @tc.desc: Receives virtual device event. if it's system's virtual device event. it's will discard.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MultiModalInputMgtServiceTest, HandleDeviceChangeEvent_008, TestSize.Level0)
{
    ClearDeviceCache();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_ = false;
    DeviceChangeEvent event;
    event.deviceChangeType = DeviceChangeType::ADD;
    event.deviceId = 3;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event);

    /*
      * Query result of the device. inputDeviceInfo's product and vendor are 0.
      * and uniq is empty.
      */
    std::pair<int32_t, InputDeviceInfo> pair;
    pair.first = 0;
    InputDeviceInfo inputDeviceInfo = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(event.deviceId);
    inputDeviceInfo.sourceTypeSet.insert(InputSourceTypeEnum::JOYSTICK);
    inputDeviceInfo.product = 0;
    inputDeviceInfo.vendor = 0;
    inputDeviceInfo.uniq = "";
    pair.second = inputDeviceInfo;
    EXPECT_CALL(*(deviceInfoServiceMock_.get()), GetInputDeviceInfo(event.deviceId)).WillOnce(Return(pair));

    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->HandleDeviceChangeEvent();

    ASSERT_TRUE(DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_);
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.size());
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.size());
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
}

static DeviceInfo BuildOldDeviceInfo()
{
    DeviceInfo oldDeviceInfo;
    oldDeviceInfo.uniq = "11:22:33:44:1";
    oldDeviceInfo.names.insert("test");
    oldDeviceInfo.ids.insert(CACHE_DEVICE_ID);
    oldDeviceInfo.hasFullKeyBoard = true;
    oldDeviceInfo.sourceTypeSet.insert(InputSourceTypeEnum::KEYBOARD);
    oldDeviceInfo.idSourceTypeMap[CACHE_DEVICE_ID] = {InputSourceTypeEnum::KEYBOARD};
    oldDeviceInfo.onlineTime = 1;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_[oldDeviceInfo.uniq] = oldDeviceInfo;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[CACHE_DEVICE_ID] = oldDeviceInfo.uniq;
    return oldDeviceInfo;
}

/**
 * @tc.name: HandleDeviceChangeEvent_009
 * @tc.desc: If the device information with the same unique ID already exists in the cache,
 * it will be updated upon receiving the device online event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MultiModalInputMgtServiceTest, HandleDeviceChangeEvent_009, TestSize.Level0)
{
    DeviceInfo oldDeviceInfo = BuildOldDeviceInfo();
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_ = false;
    DeviceChangeEvent event{};
    event.deviceChangeType = DeviceChangeType::ADD;
    event.deviceId = 1;
    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.push_back(event);
    std::pair<int32_t, InputDeviceInfo> pair;
    pair.first = 0;
    InputDeviceInfo inputDeviceInfo = MultiModalInputMgtServiceTest::CreateInputDeviceInfo(event.deviceId);
    inputDeviceInfo.sourceTypeSet.insert(InputSourceTypeEnum::JOYSTICK);
    pair.second = inputDeviceInfo;
    EXPECT_CALL(*(deviceInfoServiceMock_.get()), GetInputDeviceInfo(event.deviceId)).WillOnce(Return(pair));
    EXPECT_CALL(*(gameControllerServerClientMock_.get()), IdentifyDevice(testing::_, testing::_)).WillOnce(Return(1));

    DelayedSingleton<MultiModalInputMgtService>::GetInstance()->HandleDeviceChangeEvent();

    ASSERT_EQ(1, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_.size());
    DeviceInfo deviceInfo = DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceInfoByUniqMap_[oldDeviceInfo.uniq];
    ASSERT_TRUE(deviceInfo.hasFullKeyBoard);
    ASSERT_TRUE(deviceInfo.onlineTime != 0);
    ASSERT_TRUE(deviceInfo.ids.count(CACHE_DEVICE_ID) && deviceInfo.ids.count(event.deviceId));
    ASSERT_TRUE(deviceInfo.sourceTypeSet.count(InputSourceTypeEnum::KEYBOARD) &&
                    deviceInfo.sourceTypeSet.count(InputSourceTypeEnum::JOYSTICK));
    ASSERT_TRUE(deviceInfo.names.count("test") &&
                    deviceInfo.names.count(inputDeviceInfo.name));
    ASSERT_EQ(1, deviceInfo.idSourceTypeMap[CACHE_DEVICE_ID].count(InputSourceTypeEnum::KEYBOARD));
    ASSERT_EQ(1, deviceInfo.idSourceTypeMap[event.deviceId].count(InputSourceTypeEnum::JOYSTICK));
    ASSERT_EQ(GAME_PAD, deviceInfo.deviceType);
    ASSERT_EQ(2, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_.size());
    ASSERT_EQ(oldDeviceInfo.uniq,
              DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[event.deviceId]);
    ASSERT_EQ(oldDeviceInfo.uniq,
              DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceIdUniqMap_[CACHE_DEVICE_ID]);
    ASSERT_TRUE(DelayedSingleton<MultiModalInputMgtService>::GetInstance()->needStartDelayHandle_);
    ASSERT_EQ(0, DelayedSingleton<MultiModalInputMgtService>::GetInstance()->deviceChangeEventCache_.size());
}

}
}