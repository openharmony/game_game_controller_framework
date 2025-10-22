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

#define private public

#include "key_to_touch_manager.h"
#include "multi_modal_input_mgt_service_mock.h"

#undef private

#include <gtest/gtest.h>
#include "refbase.h"

using ::testing::Return;
using ::testing::Eq;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
namespace {
const int32_t MOUSE_LEFT_BUTTON_ID = 0;
const int32_t MOUSE_RIGHT_BUTTON_ID = 1;
const int32_t DEVICE_ID = 11;
const int32_t MAX_WIDTH = 2720;
const int32_t MAX_HEIGHT = 1260;
const int32_t KEY_CODE_UP = 2301;
const int32_t KEY_CODE_DOWN = 2302;
const int32_t KEY_CODE_LEFT = 2303;
const int32_t KEY_CODE_RIGHT = 2304;
const int32_t SINGLE_KEYMAPPING_RESULT_SIZE = 4;
const int32_t DEVICE_TYPE_KEYBOARD = 3;
const int32_t DEVICE_TYPE_HOVER_TOUCH_PAD = 2;
const size_t MAX_SINGLE_KEY_SIZE_FOR_HOVER_TOUCH_PAD = 2;
}

class KeyToTouchManagerTest : public testing::Test {
public:

    void SetUp();

    void TearDown();

    void SetKeyEvent();

    void SetPointerEvent();

    void SetDeviceInfo();

    void SetContext();

    void CheckCombinationKey(const DeviceTypeEnum &deviceType);

    void CheckDpadKeyAndKeyBoardObservation(const DeviceTypeEnum &deviceType, const MappingTypeEnum &type);

    void CheckMouseRightAndLeftButton(const DeviceTypeEnum &deviceType, const MappingTypeEnum &type);

    void CheckSingleKeyAndMouse(const DeviceTypeEnum &deviceType, const MappingTypeEnum &type);

    void CheckWindowInfo(const WindowInfoEntity &windowLeft, const WindowInfoEntity &windowRight);

    void CheckTempVariablesDefault();

public:
    std::shared_ptr<KeyToTouchManager> handler_;
    std::shared_ptr<MMI::KeyEvent> keyEvent_;
    std::shared_ptr<MMI::PointerEvent> pointerEvent_;
    std::shared_ptr<MultiModalInputMgtServiceMock> multiModalInputMgtServiceMock_;
    DeviceInfo deviceInfo_;
};

static KeyToTouchMappingInfo BuildKeyMapping(const MappingTypeEnum &mappingType)
{
    KeyToTouchMappingInfo keyMapping;
    keyMapping.mappingType = mappingType;
    switch (mappingType) {
        case MappingTypeEnum::DPAD_KEY_TO_TOUCH:
        case MappingTypeEnum::KEY_BOARD_OBSERVATION_TO_TOUCH:
            keyMapping.dpadKeyCodeEntity.up = KEY_CODE_UP;
            keyMapping.dpadKeyCodeEntity.down = KEY_CODE_DOWN;
            keyMapping.dpadKeyCodeEntity.left = KEY_CODE_LEFT;
            keyMapping.dpadKeyCodeEntity.right = KEY_CODE_RIGHT;
            break;
        case MappingTypeEnum::SKILL_KEY_TO_TOUCH:
            keyMapping.skillRange = 1;
            keyMapping.radius = 1;
            break;
        case MappingTypeEnum::COMBINATION_KEY_TO_TOUCH:
            keyMapping.combinationKeys = {0, 1};
            break;
        default:
            break;
    }
    return keyMapping;
}

void KeyToTouchManagerTest::SetUp()
{
    multiModalInputMgtServiceMock_ = std::make_shared<MultiModalInputMgtServiceMock>();
    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    handler_ = std::make_shared<KeyToTouchManager>();
    handler_->allMonitorKeys_[KEY_CODE_UP].insert(DeviceTypeEnum::GAME_KEY_BOARD);

    SetKeyEvent();
    SetPointerEvent();
    SetDeviceInfo();

    handler_->isSupportKeyMapping_ = true;
    handler_->isMonitorMouse_ = true;
    handler_->windowInfoEntity_.isFullScreen = true;
    handler_->isEnableKeyMapping_ = true;
}

void KeyToTouchManagerTest::TearDown()
{
    multiModalInputMgtServiceMock_.reset();
    MultiModalInputMgtService::instance_ = nullptr;
}

void KeyToTouchManagerTest::SetKeyEvent()
{
    keyEvent_ = MMI::KeyEvent::Create();
    keyEvent_->SetKeyCode(KEY_CODE_UP);
    keyEvent_->SetDeviceId(DEVICE_ID);
    keyEvent_->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
}

void KeyToTouchManagerTest::SetPointerEvent()
{
    pointerEvent_ = MMI::PointerEvent::Create();
    pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent_->SetButtonId(MOUSE_LEFT_BUTTON_ID);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
}

void KeyToTouchManagerTest::SetDeviceInfo()
{
    deviceInfo_.uniq = "notnull";
    deviceInfo_.deviceType = DeviceTypeEnum::GAME_KEY_BOARD;
}

void KeyToTouchManagerTest::SetContext()
{
    std::vector<KeyToTouchMappingInfo> testMapping;
    KeyToTouchMappingInfo info = BuildKeyMapping(MappingTypeEnum::SINGE_KEY_TO_TOUCH);
    info.keyCode = KEY_CODE_UP;
    testMapping.push_back(info);
    handler_->InitGcKeyboardContext(testMapping);
    handler_->gcKeyboardContext_->deviceType = DeviceTypeEnum::GAME_KEY_BOARD;

    // set current temp variables
    handler_->gcKeyboardContext_->isSingleKeyOperating = true;
    handler_->gcKeyboardContext_->currentSingleKey = BuildKeyMapping(MappingTypeEnum::SINGE_KEY_TO_TOUCH);
    handler_->gcKeyboardContext_->currentSingleKey.keyCode = KEY_CODE_UP;
    handler_->gcKeyboardContext_->isCombinationKeyOperating = true;
    handler_->gcKeyboardContext_->currentCombinationKey = BuildKeyMapping(MappingTypeEnum::COMBINATION_KEY_TO_TOUCH);
    handler_->gcKeyboardContext_->isSkillOperating = true;
    handler_->gcKeyboardContext_->currentSkillKeyInfo = BuildKeyMapping(MappingTypeEnum::SKILL_KEY_TO_TOUCH);
}

void KeyToTouchManagerTest::CheckCombinationKey(const DeviceTypeEnum &deviceType)
{
    // mapping type should be COMBINATION_KEY_TO_TOUCH
    std::vector<KeyToTouchMappingInfo> testMappingInfos;
    testMappingInfos.push_back(BuildKeyMapping(MappingTypeEnum::COMBINATION_KEY_TO_TOUCH));
    handler_->HandleTemplateConfig(deviceType, testMappingInfos);

    std::shared_ptr<InputToTouchContext> context =
        deviceType == DeviceTypeEnum::GAME_KEY_BOARD ? handler_->gcKeyboardContext_ : handler_->hoverTouchPadContext_;

    ASSERT_FALSE(context == nullptr);
    ASSERT_EQ(context->combinationKeyMappings.size(), 1);
    ASSERT_TRUE(context->combinationKeyMappings.find(1) != context->combinationKeyMappings.end());
    ASSERT_EQ(context->combinationKeyMappings[1].size(), 1);
    ASSERT_TRUE(context->combinationKeyMappings[1].find(0) != context->combinationKeyMappings[1].end());
    ASSERT_EQ(context->combinationKeyMappings[1][0].mappingType, MappingTypeEnum::COMBINATION_KEY_TO_TOUCH);
    ASSERT_FALSE(handler_->isMonitorMouse_);
    ASSERT_TRUE(handler_->allMonitorKeys_[1].find(deviceType) != handler_->allMonitorKeys_[1].end());
}

void KeyToTouchManagerTest::CheckDpadKeyAndKeyBoardObservation(const DeviceTypeEnum &deviceType,
                                                               const MappingTypeEnum &type)
{
    // type can be DPAD_KEY_TO_TOUCH and KEY_BOARD_OBSERVATION_TO_TOUCH
    std::vector<KeyToTouchMappingInfo> testMappingInfos;
    testMappingInfos.push_back(BuildKeyMapping(type));
    handler_->HandleTemplateConfig(deviceType, testMappingInfos);

    std::shared_ptr<InputToTouchContext> context =
        deviceType == DeviceTypeEnum::GAME_KEY_BOARD ? handler_->gcKeyboardContext_ : handler_->hoverTouchPadContext_;

    ASSERT_FALSE(context == nullptr);
    ASSERT_EQ(context->singleKeyMappings.size(), SINGLE_KEYMAPPING_RESULT_SIZE);
    ASSERT_TRUE(context->singleKeyMappings.find(KEY_CODE_UP) != context->singleKeyMappings.end());
    ASSERT_EQ(context->singleKeyMappings[KEY_CODE_UP].mappingType, type);
    ASSERT_TRUE(context->singleKeyMappings.find(KEY_CODE_DOWN) != context->singleKeyMappings.end());
    ASSERT_EQ(context->singleKeyMappings[KEY_CODE_DOWN].mappingType, type);
    ASSERT_TRUE(context->singleKeyMappings.find(KEY_CODE_LEFT) != context->singleKeyMappings.end());
    ASSERT_EQ(context->singleKeyMappings[KEY_CODE_LEFT].mappingType, type);
    ASSERT_TRUE(context->singleKeyMappings.find(KEY_CODE_RIGHT) != context->singleKeyMappings.end());
    ASSERT_EQ(context->singleKeyMappings[KEY_CODE_RIGHT].mappingType, type);
    ASSERT_FALSE(handler_->isMonitorMouse_);
    ASSERT_TRUE(
        handler_->allMonitorKeys_[KEY_CODE_UP].find(deviceType) != handler_->allMonitorKeys_[KEY_CODE_UP].end());
    ASSERT_TRUE(
        handler_->allMonitorKeys_[KEY_CODE_DOWN].find(deviceType) != handler_->allMonitorKeys_[KEY_CODE_DOWN].end());
    ASSERT_TRUE(
        handler_->allMonitorKeys_[KEY_CODE_LEFT].find(deviceType) != handler_->allMonitorKeys_[KEY_CODE_LEFT].end());
    ASSERT_TRUE(
        handler_->allMonitorKeys_[KEY_CODE_RIGHT].find(deviceType) != handler_->allMonitorKeys_[KEY_CODE_RIGHT].end());
}

void KeyToTouchManagerTest::CheckMouseRightAndLeftButton(const DeviceTypeEnum &deviceType, const MappingTypeEnum &type)
{
    // type can be MOUSE_RIGHT_KEY_WALKING_TO_TOUCH , MOUSE_OBSERVATION_TO_TOUCH and MOUSE_LEFT_FIRE_TO_TOUCH
    std::vector<KeyToTouchMappingInfo> testMappingInfos;
    testMappingInfos.push_back(BuildKeyMapping(type));
    handler_->HandleTemplateConfig(deviceType, testMappingInfos);

    std::shared_ptr<InputToTouchContext> context =
        deviceType == DeviceTypeEnum::GAME_KEY_BOARD ? handler_->gcKeyboardContext_ : handler_->hoverTouchPadContext_;

    ASSERT_FALSE(context == nullptr);
    ASSERT_EQ(context->mouseBtnKeyMappings.size(), 1);
    int32_t keyCode = type == MappingTypeEnum::MOUSE_LEFT_FIRE_TO_TOUCH ? MOUSE_LEFT_BUTTON_KEYCODE :
        MOUSE_RIGHT_BUTTON_KEYCODE;
    ASSERT_TRUE(context->mouseBtnKeyMappings.find(keyCode) != context->mouseBtnKeyMappings.end());
    ASSERT_EQ(context->mouseBtnKeyMappings[keyCode].mappingType, type);

    ASSERT_TRUE(context->isMonitorMouse);
    ASSERT_TRUE(handler_->isMonitorMouse_);

    ASSERT_TRUE(handler_->allMonitorKeys_[keyCode].find(deviceType) != handler_->allMonitorKeys_[keyCode].end());
}

void KeyToTouchManagerTest::CheckSingleKeyAndMouse(const DeviceTypeEnum &deviceType, const MappingTypeEnum &type)
{
    // type can be SKILL_KEY_TO_TOUCH,CROSSHAIR_KEY_TO_TOUCH, OBSERVATION_KEY_TO_TOUCH and SINGE_KEY_TO_TOUCH
    std::vector<KeyToTouchMappingInfo> testMappingInfos;
    testMappingInfos.push_back(BuildKeyMapping(type));
    handler_->HandleTemplateConfig(deviceType, testMappingInfos);

    std::shared_ptr<InputToTouchContext> context =
        deviceType == DeviceTypeEnum::GAME_KEY_BOARD ? handler_->gcKeyboardContext_ : handler_->hoverTouchPadContext_;

    ASSERT_FALSE(context == nullptr);
    ASSERT_EQ(context->singleKeyMappings.size(), 1);
    ASSERT_TRUE(context->singleKeyMappings.find(0) != context->singleKeyMappings.end());
    ASSERT_EQ(context->singleKeyMappings[0].mappingType, type);
    if (type != MappingTypeEnum::SINGE_KEY_TO_TOUCH) {
        ASSERT_TRUE(context->isMonitorMouse);
        ASSERT_TRUE(handler_->isMonitorMouse_);
    } else {
        ASSERT_FALSE(context->isMonitorMouse);
        ASSERT_FALSE(handler_->isMonitorMouse_);
    }
    ASSERT_TRUE(handler_->allMonitorKeys_[0].find(deviceType) != handler_->allMonitorKeys_[0].end());
}

void KeyToTouchManagerTest::CheckWindowInfo(const WindowInfoEntity &windowLeft, const WindowInfoEntity &windowRight)
{
    ASSERT_EQ(windowLeft.windowId, windowRight.windowId);
    ASSERT_EQ(windowLeft.maxWidth, windowRight.maxWidth);
    ASSERT_EQ(windowLeft.maxHeight, windowRight.maxHeight);
    ASSERT_EQ(windowLeft.currentWidth, windowRight.currentWidth);
    ASSERT_EQ(windowLeft.currentHeight, windowRight.currentHeight);
    ASSERT_EQ(windowLeft.xPosition, windowRight.xPosition);
    ASSERT_EQ(windowLeft.yPosition, windowRight.yPosition);
    ASSERT_EQ(windowLeft.isFullScreen, windowRight.isFullScreen);
    ASSERT_EQ(windowLeft.xCenter, windowRight.xCenter);
    ASSERT_EQ(windowLeft.yCenter, windowRight.yCenter);
}

void KeyToTouchManagerTest::CheckTempVariablesDefault()
{
    ASSERT_FALSE(handler_->gcKeyboardContext_->isSingleKeyOperating);
    ASSERT_EQ(handler_->gcKeyboardContext_->currentSingleKey.keyCode, 0);
    ASSERT_FALSE(handler_->gcKeyboardContext_->isCombinationKeyOperating);
    ASSERT_EQ(handler_->gcKeyboardContext_->currentCombinationKey.combinationKeys.size(), 0);
    ASSERT_FALSE(handler_->gcKeyboardContext_->isSkillOperating);
    ASSERT_EQ(handler_->gcKeyboardContext_->currentSkillKeyInfo.skillRange, 0);
    ASSERT_EQ(handler_->gcKeyboardContext_->currentSkillKeyInfo.radius, 0);
}

/**
 * @tc.name: SetSupportKeyMapping_001
 * @tc.desc: when SetSupportKeyMapping function is called, isSupportKeyMapping_ is set true;
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, SetSupportKeyMapping_001, TestSize.Level0)
{
    std::unordered_set<int32_t> deviceTypeSet;
    deviceTypeSet.insert(DEVICE_TYPE_KEYBOARD);
    handler_->SetSupportKeyMapping(true, deviceTypeSet);
    ASSERT_TRUE(handler_->isSupportKeyMapping_);
    ASSERT_TRUE(handler_->supportDeviceTypeSet_.count(DEVICE_TYPE_KEYBOARD) == 1);
    handler_->SetSupportKeyMapping(false, deviceTypeSet);
    ASSERT_TRUE(handler_->supportDeviceTypeSet_.count(DEVICE_TYPE_HOVER_TOUCH_PAD) == 1);
    ASSERT_FALSE(handler_->isSupportKeyMapping_);
}

/**
 * @tc.name: HandleTemplateConfig_001
 * @tc.desc: when HandleTemplateConfig is called,COMBINATION_KEY_TO_TOUCH type mappingInfos are insert into the
 *           combinationKeyMappings of the context
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleTemplateConfig_001, TestSize.Level0)
{
    CheckCombinationKey(DeviceTypeEnum::GAME_KEY_BOARD);
}

/**
 * @tc.name: HandleTemplateConfig_002
 * @tc.desc: when HandleTemplateConfig is called,DPAD_KEY_TO_TOUCH type mappingInfos are insert into the
 *           singleKeyMappings of the context
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleTemplateConfig_002, TestSize.Level0)
{
    CheckDpadKeyAndKeyBoardObservation(DeviceTypeEnum::GAME_KEY_BOARD, MappingTypeEnum::DPAD_KEY_TO_TOUCH);
}

/**
 * @tc.name: HandleTemplateConfig_003
 * @tc.desc: when HandleTemplateConfig is called,KEY_BOARD_OBSERVATION_TO_TOUCH type mappingInfos are insert into the
 *           singleKeyMappings of the contex
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleTemplateConfig_003, TestSize.Level0)
{
    CheckDpadKeyAndKeyBoardObservation(DeviceTypeEnum::GAME_KEY_BOARD, MappingTypeEnum::KEY_BOARD_OBSERVATION_TO_TOUCH);
}

/**
 * @tc.name: HandleTemplateConfig_004
 * @tc.desc: when HandleTemplateConfig is called,MOUSE_RIGHT_KEY_WALKING_TO_TOUCH type mappingInfos are insert into the
 *           singleKeyMappings and mouseBtnKeyMappings of the contex
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleTemplateConfig_004, TestSize.Level0)
{
    CheckMouseRightAndLeftButton(DeviceTypeEnum::GAME_KEY_BOARD, MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH);
}

/**
 * @tc.name: HandleTemplateConfig_005
 * @tc.desc: when HandleTemplateConfig is called,MOUSE_OBSERVATION_TO_TOUCH type mappingInfos are insert into the
 *           singleKeyMappings and mouseBtnKeyMappings of the contex
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleTemplateConfig_005, TestSize.Level0)
{
    CheckMouseRightAndLeftButton(DeviceTypeEnum::GAME_KEY_BOARD, MappingTypeEnum::MOUSE_OBSERVATION_TO_TOUCH);
}

/**
 * @tc.name: HandleTemplateConfig_006
 * @tc.desc: when HandleTemplateConfig is called,MOUSE_LEFT_FIRE_TO_TOUCH type mappingInfos are insert into the
 *           mouseBtnKeyMappings of the contex
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleTemplateConfig_006, TestSize.Level0)
{
    CheckMouseRightAndLeftButton(DeviceTypeEnum::GAME_KEY_BOARD, MappingTypeEnum::MOUSE_LEFT_FIRE_TO_TOUCH);
}

/**
 * @tc.name: HandleTemplateConfig_007
 * @tc.desc: when HandleTemplateConfig is called,SKILL_KEY_TO_TOUCH type mappingInfos are insert into the
 *           singleKeyMappings of the contex
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleTemplateConfig_007, TestSize.Level0)
{
    CheckSingleKeyAndMouse(DeviceTypeEnum::GAME_KEY_BOARD, MappingTypeEnum::SKILL_KEY_TO_TOUCH);
}

/**
 * @tc.name: HandleTemplateConfig_008
 * @tc.desc: when HandleTemplateConfig is called,CROSSHAIR_KEY_TO_TOUCH type mappingInfos are insert into the
 *           singleKeyMappings of the contex
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleTemplateConfig_008, TestSize.Level0)
{
    CheckSingleKeyAndMouse(DeviceTypeEnum::GAME_KEY_BOARD, MappingTypeEnum::CROSSHAIR_KEY_TO_TOUCH);
}

/**
 * @tc.name: HandleTemplateConfig_009
 * @tc.desc: when HandleTemplateConfig is called,OBSERVATION_KEY_TO_TOUCH type mappingInfos are insert into the
 *           singleKeyMappings of the contex
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleTemplateConfig_009, TestSize.Level0)
{
    CheckSingleKeyAndMouse(DeviceTypeEnum::GAME_KEY_BOARD, MappingTypeEnum::OBSERVATION_KEY_TO_TOUCH);
}

/**
 * @tc.name: HandleTemplateConfig_010
 * @tc.desc: when HandleTemplateConfig is called,SINGE_KEY_TO_TOUCH type mappingInfos are insert into the
 *           singleKeyMappings of the contex
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleTemplateConfig_010, TestSize.Level0)
{
    CheckSingleKeyAndMouse(DeviceTypeEnum::GAME_KEY_BOARD, MappingTypeEnum::SINGE_KEY_TO_TOUCH);
    CheckSingleKeyAndMouse(DeviceTypeEnum::HOVER_TOUCH_PAD, MappingTypeEnum::SINGE_KEY_TO_TOUCH);
}

/**
 * @tc.name: HandleTemplateConfig_011
 * @tc.desc: when HandleTemplateConfig is called,MOUSE_RIGHT_KEY_CLICK_TO_TOUCH type mappingInfos are insert into the
 *           mouseBtnKeyMappings of the context
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleTemplateConfig_011, TestSize.Level0)
{
    CheckMouseRightAndLeftButton(DeviceTypeEnum::GAME_KEY_BOARD, MappingTypeEnum::MOUSE_RIGHT_KEY_CLICK_TO_TOUCH);
}

/**
 * @tc.name: DispatchKeyEvent_001
 * @tc.desc: when DispatchKeyEvent function is called, the invalid keyEvent is filtered
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, DispatchKeyEvent_001, TestSize.Level0)
{
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).Times(1).WillOnce(
        Return(deviceInfo_));
    ASSERT_TRUE(handler_->DispatchKeyEvent(keyEvent_));
    handler_->isSupportKeyMapping_ = false;
    ASSERT_FALSE(handler_->DispatchKeyEvent(keyEvent_));
}

/**
 * @tc.name: DispatchKeyEvent_002
 * @tc.desc: when KEY_ACTION is invalid ,DispatchKeyEvent_ return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, DispatchKeyEvent_002, TestSize.Level0)
{
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).WillOnce(
        Return(deviceInfo_));
    ASSERT_TRUE(handler_->DispatchKeyEvent(keyEvent_));
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UNKNOWN);
    ASSERT_FALSE(handler_->DispatchKeyEvent(keyEvent_));
}

/**
 * @tc.name: DispatchKeyEvent_003
 * @tc.desc: when keycode is not in allMonitorKeys_, DispatchKeyEvent_ return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, DispatchKeyEvent_003, TestSize.Level0)
{
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).WillOnce(
        Return(deviceInfo_));
    ASSERT_TRUE(handler_->DispatchKeyEvent(keyEvent_));
    handler_->allMonitorKeys_.clear();
    handler_->allMonitorKeys_[KEY_CODE_DOWN].insert(DeviceTypeEnum::GAME_KEY_BOARD);
    ASSERT_FALSE(handler_->DispatchKeyEvent(keyEvent_));
}

/**
 * @tc.name: DispatchKeyEvent_004
 * @tc.desc: when deviceInfo.uniq of key event is empty, DispatchKeyEvent_ return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, DispatchKeyEvent_004, TestSize.Level0)
{
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).WillOnce(
        Return(deviceInfo_));
    ASSERT_TRUE(handler_->DispatchKeyEvent(keyEvent_));
    deviceInfo_.uniq = "null";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).WillOnce(
        Return(deviceInfo_));
    ASSERT_FALSE(handler_->DispatchKeyEvent(keyEvent_));
}

/**
 * @tc.name: DispatchKeyEvent_005
 * @tc.desc: when deviceInfo.type of key event is unknown and keyboard type is monitored, DispatchKeyEvent_ return true
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, DispatchKeyEvent_005, TestSize.Level0)
{
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).WillOnce(
        Return(deviceInfo_));
    ASSERT_TRUE(handler_->DispatchKeyEvent(keyEvent_));
    deviceInfo_.deviceType = DeviceTypeEnum::UNKNOWN;
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).WillOnce(
        Return(deviceInfo_));
    ASSERT_TRUE(handler_->DispatchKeyEvent(keyEvent_));
    handler_->allMonitorKeys_[KEY_CODE_UP].clear();
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).WillOnce(
        Return(deviceInfo_));
    ASSERT_FALSE(handler_->DispatchKeyEvent(keyEvent_));
}

/**
 * @tc.name: DispatchKeyEvent_006
 * @tc.desc: when deviceInfo.type of key event is not unknown and device type is monitored in allMonitorKeys,
 *           DispatchKeyEvent_ return true
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, DispatchKeyEvent_006, TestSize.Level0)
{
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).WillOnce(
        Return(deviceInfo_));
    ASSERT_TRUE(handler_->DispatchKeyEvent(keyEvent_));
    handler_->allMonitorKeys_[KEY_CODE_UP].clear();
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent_->GetDeviceId())).WillOnce(
        Return(deviceInfo_));
    ASSERT_FALSE(handler_->DispatchKeyEvent(keyEvent_));
}

/**
 * @tc.name: DispatchPointerEvent_001
 * @tc.desc: when isSupportKeyMapping_ is false, DispatchPointerEvent function return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, DispatchPointerEvent_001, TestSize.Level0)
{
    ASSERT_TRUE(handler_->DispatchPointerEvent(pointerEvent_));
    handler_->isSupportKeyMapping_ = false;
    ASSERT_FALSE(handler_->DispatchPointerEvent(pointerEvent_));
}

/**
 * @tc.name: DispatchPointerEvent_002
 * @tc.desc: when DispatchPointerEvent function is called, the invalid pointerEvent is filtered
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, DispatchPointerEvent_002, TestSize.Level0)
{
    ASSERT_TRUE(handler_->DispatchPointerEvent(pointerEvent_));
    pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_UNKNOWN);
    ASSERT_FALSE(handler_->DispatchPointerEvent(pointerEvent_));
    pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_UNKNOWN);
    ASSERT_FALSE(handler_->DispatchPointerEvent(pointerEvent_));
    pointerEvent_->SetButtonId(MOUSE_RIGHT_BUTTON_ID);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_UNKNOWN);
    ASSERT_FALSE(handler_->DispatchPointerEvent(pointerEvent_));
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_TRUE(handler_->DispatchPointerEvent(pointerEvent_));
}

/**
 * @tc.name: DispatchPointerEvent_003
 * @tc.desc: when isMonitorMouse_ is false, DispatchPointerEvent function return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, DispatchPointerEvent_003, TestSize.Level0)
{
    ASSERT_TRUE(handler_->DispatchPointerEvent(pointerEvent_));
    handler_->isMonitorMouse_ = false;
    ASSERT_FALSE(handler_->DispatchPointerEvent(pointerEvent_));
}

/**
 * @tc.name: HandleWindowInfo_001
 * @tc.desc: when HandleWindowInfo function is called, window info is set to
 *           gcKeyboardContext_ and hoverTouchPadContext_;
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleWindowInfo_001, TestSize.Level0)
{
    std::vector<KeyToTouchMappingInfo> testMappingInfos;
    testMappingInfos.push_back(BuildKeyMapping(MappingTypeEnum::SINGE_KEY_TO_TOUCH));
    WindowInfoEntity window = {1, 100, 100, 100, 100, 10, 10, true, 25, 25};

    handler_->HandleWindowInfo(window);
    CheckWindowInfo(handler_->windowInfoEntity_, window);

    handler_->InitGcKeyboardContext(testMappingInfos);
    handler_->InitHoverTouchPadContext(testMappingInfos);
    handler_->HandleWindowInfo(window);

    CheckWindowInfo(handler_->gcKeyboardContext_->windowInfoEntity, window);
    CheckWindowInfo(handler_->hoverTouchPadContext_->windowInfoEntity, window);
}

/**
 * @tc.name: HandleEnableKeyMapping_001
 * @tc.desc: when HandleEnableKeyMapping function is called, isEnableKeyMapping_ is set
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleEnableKeyMapping_001, TestSize.Level0)
{
    ASSERT_TRUE(handler_->isEnableKeyMapping_);
    handler_->HandleEnableKeyMapping(false);
    ASSERT_FALSE(handler_->isEnableKeyMapping_);
}

/**
 * @tc.name: HandleEnableKeyMapping_001
 * @tc.desc: when HandleEnableKeyMapping function is called, current TempVariables in context is reset
 *  to default
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyToTouchManagerTest, HandleEnableKeyMapping_002, TestSize.Level0)
{
    SetContext();
    handler_->HandleEnableKeyMapping(true);
    CheckTempVariablesDefault();
}
}
}