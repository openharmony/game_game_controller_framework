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
#include <gmock/gmock.h>
#include <gmock/gmock-actions.h>
#include <gmock/gmock-spec-builders.h>
#include <gtest/gtest.h>
#include "refbase.h"
#include "key_event.h"

#define private public

#include "key_mapping_handle.h"
#include "multi_modal_input_mgt_service_mock.h"

#undef private

using ::testing::Return;
using ::testing::Eq;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
namespace {
const int64_t WRONG_DOWN_TIME = 999;
const int64_t DEVICE_ONLINE_TIME = 1000;
const int64_t KEYCODE_CTRL_DOWN_TIME = 1100;
const int64_t KEYCODE_SHIFT_DOWN_TIME = 1200;
const int64_t KEYCODE_I_DOWN_TIME = 1300;

const int32_t DEVICE_ID = 11;

const int32_t KEY_CODE_UP = 2301;
const int32_t KEY_CODE_DOWN = 2302;
const int32_t KEY_CODE_LEFT = 2303;
const int32_t KEY_CODE_RIGHT = 2304;

const int32_t KEYCODE_OPEN_TEMP_FOR_HOVER_TOUCH_CONTROLLER = 3107;
}

class KeyMappingHandleTest : public testing::Test {
public:

    void SetUp() override;

    void TearDown() override;

    void SetKeyEvent();

    void SetDeviceInfo();

public:
    std::shared_ptr<MultiModalInputMgtServiceMock> multiModalInputMgtServiceMock_;
    std::shared_ptr<KeyMappingHandle> handler_;
    std::shared_ptr<MMI::KeyEvent> keyEvent_;
    DeviceInfo deviceInfo_;
};

void KeyMappingHandleTest::SetUp()
{
    multiModalInputMgtServiceMock_ = std::make_shared<MultiModalInputMgtServiceMock>();
    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    handler_ = std::make_shared<KeyMappingHandle>();
    handler_->SetSupportKeyMapping(true);
    SetKeyEvent();
    SetDeviceInfo();
}

void KeyMappingHandleTest::TearDown()
{
    multiModalInputMgtServiceMock_.reset();
    MultiModalInputMgtService::instance_ = nullptr;
}

void KeyMappingHandleTest::SetKeyEvent()
{
    keyEvent_ = MMI::KeyEvent::Create();
    MMI::KeyEvent::KeyItem keyItem;

    keyItem.SetKeyCode(MMI::KeyEvent::KEYCODE_CTRL_LEFT);
    keyItem.SetDownTime(KEYCODE_CTRL_DOWN_TIME);
    keyItem.SetPressed(true);
    keyItem.SetDeviceId(DEVICE_ID);
    keyEvent_->AddKeyItem(keyItem);

    keyItem.SetKeyCode(MMI::KeyEvent::KEYCODE_SHIFT_LEFT);
    keyItem.SetDownTime(KEYCODE_SHIFT_DOWN_TIME);
    keyEvent_->AddKeyItem(keyItem);

    keyItem.SetKeyCode(MMI::KeyEvent::KEYCODE_I);
    keyItem.SetDownTime(KEYCODE_I_DOWN_TIME);
    keyEvent_->AddKeyItem(keyItem);

    keyEvent_->SetDeviceId(DEVICE_ID);
    keyEvent_->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
}

void KeyMappingHandleTest::SetDeviceInfo()
{
    deviceInfo_.onlineTime = DEVICE_ONLINE_TIME;
    deviceInfo_.uniq = "notnull";
    deviceInfo_.deviceType = DeviceTypeEnum::GAME_KEY_BOARD;

    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).WillRepeatedly(
        Return(deviceInfo_));
}

/**
 * @tc.name: SetSupportKeyMapping_001
 * @tc.desc: When SetSupportKeyMapping function is called, isSupportKeyMapping_ is set true.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, SetSupportKeyMapping_001, TestSize.Level0)
{
    ASSERT_TRUE(handler_->isSupportKeyMapping_);
    handler_->SetSupportKeyMapping(false);
    ASSERT_FALSE(handler_->isSupportKeyMapping_);
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_001
 * @tc.desc: When isSupportKeyMapping_ is false, IsNotifyOpenTemplateConfigPage return false.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_001, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    handler_->SetSupportKeyMapping(false);
    ASSERT_FALSE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_001
 * @tc.desc: When key action of keyEventA_ is not KEY_ACTION_DOWN, IsNotifyOpenTemplateConfigPage return false.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_002, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    keyEvent_->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    ASSERT_FALSE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_003
 * @tc.desc: When uniq of deviceInfo_ is empty, IsNotifyOpenTemplateConfigPage return false.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_003, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    deviceInfo_.uniq = "null";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).Times(1).WillOnce(
        Return(deviceInfo_));
    ASSERT_FALSE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_004
 * @tc.desc: When device type is not GAME_KEY_BOARD or HOVER_TOUCH_PAD, IsNotifyOpenTemplateConfigPage return false.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_004, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    deviceInfo_.deviceType = DeviceTypeEnum::UNKNOWN;
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).Times(1).WillOnce(
        Return(deviceInfo_));
    ASSERT_FALSE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_005
 * @tc.desc: Device type of deviceInfo_ is HOVER_TOUCH_PAD. When keycode is not
 * KEYCODE_OPEN_TEMP_FOR_HOVER_TOUCH_CONTROLLER, IsNotifyOpenTemplateConfigPage return false.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_005, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    deviceInfo_.deviceType = DeviceTypeEnum::HOVER_TOUCH_PAD;
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).
        Times(2).WillRepeatedly(Return(deviceInfo_));

    keyEvent_->SetKeyCode(KEYCODE_OPEN_TEMP_FOR_HOVER_TOUCH_CONTROLLER);
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));

    keyEvent_->SetKeyCode(KEYCODE_OPEN_TEMP_FOR_HOVER_TOUCH_CONTROLLER + 1);
    ASSERT_FALSE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_006
 * @tc.desc: When keyItems are more than ctrl+shift+i, IsNotifyOpenTemplateConfigPage return false.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_006, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    MMI::KeyEvent::KeyItem newItem;
    newItem.SetKeyCode(MMI::KeyEvent::KEYCODE_O);
    newItem.SetDownTime(KEYCODE_I_DOWN_TIME);
    newItem.SetDeviceId(DEVICE_ID);
    newItem.SetPressed(true);
    keyEvent_->AddKeyItem(newItem);
    ASSERT_FALSE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_007
 * @tc.desc: When one of ctrl+shift+i keys is not pressed, IsNotifyOpenTemplateConfigPage return false.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_007, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    std::vector<MMI::KeyEvent::KeyItem> items = keyEvent_->GetKeyItems();
    ASSERT_EQ(items.size(), 3);
    items[0].SetPressed(false);
    keyEvent_->SetKeyItem(items);
    ASSERT_FALSE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_008
 * @tc.desc: When one deviceId of ctrl+shift+i keys is not same with keyEventA_, IsNotifyOpenTemplateConfigPage return
 * false.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_008, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    std::vector<MMI::KeyEvent::KeyItem> items = keyEvent_->GetKeyItems();
    ASSERT_EQ(items.size(), 3);
    items[0].SetDeviceId(DEVICE_ID + 1);
    keyEvent_->SetKeyItem(items);
    ASSERT_FALSE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_009
 * @tc.desc: When one's down time of ctrl+shift+i keys is earlier than device online time,
 * IsNotifyOpenTemplateConfigPage return false.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_009, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    std::vector<MMI::KeyEvent::KeyItem> items = keyEvent_->GetKeyItems();
    ASSERT_EQ(items.size(), 3);
    items[0].SetDownTime(WRONG_DOWN_TIME);
    keyEvent_->SetKeyItem(items);
    ASSERT_FALSE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_010
 * @tc.desc: When ctrl+shift+i are pressed down and deviceType is unknown and hasFullKeyBoard is true,
 * IsNotifyOpenTemplateConfigPage return true.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_010, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    deviceInfo_.deviceType = DeviceTypeEnum::UNKNOWN;
    deviceInfo_.hasFullKeyBoard = true;
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).
        Times(1).WillOnce(Return(deviceInfo_));
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_011
 * @tc.desc: When ctrl+shift+i are pressed down and deviceType is unknown and hasFullKeyBoard is false,
 * IsNotifyOpenTemplateConfigPage return false.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_011, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    deviceInfo_.deviceType = DeviceTypeEnum::UNKNOWN;
    deviceInfo_.hasFullKeyBoard = false;
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).
        Times(1).WillOnce(Return(deviceInfo_));
    ASSERT_FALSE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_012
 * @tc.desc: When ctrl+shift+i are pressed, no matter whether ctrl/shift is left or right,
 * IsNotifyOpenTemplateConfigPage return true.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_012, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    std::vector<MMI::KeyEvent::KeyItem> items = keyEvent_->GetKeyItems();
    ASSERT_EQ(items.size(), 3);
    ASSERT_EQ(items[0].GetKeyCode(), MMI::KeyEvent::KEYCODE_CTRL_LEFT);
    items[0].SetKeyCode(MMI::KeyEvent::KEYCODE_CTRL_RIGHT);
    keyEvent_->SetKeyItem(items);
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_013
 * @tc.desc: When ctrl+shift+i are pressed, no matter whether ctrl/shift is left or right,
 * IsNotifyOpenTemplateConfigPage return true.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_013, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    std::vector<MMI::KeyEvent::KeyItem> items = keyEvent_->GetKeyItems();
    ASSERT_EQ(items.size(), 3);
    ASSERT_EQ(items[1].GetKeyCode(), MMI::KeyEvent::KEYCODE_SHIFT_LEFT);
    items[1].SetKeyCode(MMI::KeyEvent::KEYCODE_SHIFT_RIGHT);
    keyEvent_->SetKeyItem(items);
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}

/**
 * @tc.name: IsNotifyOpenTemplateConfigPage_014
 * @tc.desc: When ctrl+shift+i are pressed, no matter whether ctrl/shift is left or right,
 * IsNotifyOpenTemplateConfigPage return true.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingHandleTest, IsNotifyOpenTemplateConfigPage_014, TestSize.Level0)
{
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
    std::vector<MMI::KeyEvent::KeyItem> items = keyEvent_->GetKeyItems();
    ASSERT_EQ(items.size(), 3);
    ASSERT_EQ(items[0].GetKeyCode(), MMI::KeyEvent::KEYCODE_CTRL_LEFT);
    items[0].SetKeyCode(MMI::KeyEvent::KEYCODE_CTRL_RIGHT);
    ASSERT_EQ(items[1].GetKeyCode(), MMI::KeyEvent::KEYCODE_SHIFT_LEFT);
    items[1].SetKeyCode(MMI::KeyEvent::KEYCODE_SHIFT_RIGHT);
    keyEvent_->SetKeyItem(items);
    ASSERT_TRUE(handler_->IsNotifyOpenTemplateConfigPage(keyEvent_));
}
}
}
