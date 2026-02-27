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
#include <gtest/gtest.h>

#define private public

#include "key_to_touch_handler.h"

#undef private

#include "refbase.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
namespace {
const int32_t X_VALUE = 2045;
const int32_t Y_VALUE = 1003;
const int32_t KEY_CODE_A = 2301;
const int32_t KEY_CODE_B = 2302;
const int32_t DEVICE_ID = 11;
const int32_t WINDOW_ID = 1;
const int32_t MAX_WIDTH = 2720;
const int32_t MAX_HEIGHT = 1260;
const int32_t MOUSE_LEFT_BUTTON_ID = 0;
const int32_t MOUSE_RIGHT_BUTTON_ID = 1;
const int32_t POINTER_ID_START = 3;
const int32_t RADIUS = 100;
}

class TestKeyToTouchHandlerEx : public BaseKeyToTouchHandler {
public:
    TestKeyToTouchHandlerEx() = default;

    void BuildAndSendPointerEvent(std::shared_ptr<InputToTouchContext> &context,
                                  const TouchEntity &touchEntity) override
    {
        BaseKeyToTouchHandler::BuildAndSendPointerEvent(context, touchEntity);
        touchEntity_ = touchEntity;
    }

    void HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                       const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                       const KeyToTouchMappingInfo &mappingInfo, const DeviceInfo &deviceInfo) override
    {
        BaseKeyToTouchHandler::HandleKeyDown(context, keyEvent, mappingInfo, deviceInfo);
    }

    void HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                     const std::shared_ptr<MMI::KeyEvent> &keyEvent, const DeviceInfo &deviceInfo) override
    {
        BaseKeyToTouchHandler::HandleKeyUp(context, keyEvent, deviceInfo);
    }

    void HandlePointerEvent(std::shared_ptr<InputToTouchContext> &context,
                            const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                            const KeyToTouchMappingInfo &mappingInfo) override
    {
        BaseKeyToTouchHandler::HandlePointerEvent(context, pointerEvent, mappingInfo);
    }

    void ExitCrosshairKeyStatus(const std::shared_ptr<InputToTouchContext> &context) override
    {
        BaseKeyToTouchHandler::ExitCrosshairKeyStatus(context);
    }

public:
    TouchEntity touchEntity_;
};

class BaseKeyToTouchHandlerTest : public testing::Test {
public:
    void SetUp() override
    {
        handler_ = std::make_shared<TestKeyToTouchHandlerEx>();
        context_ = std::make_shared<InputToTouchContext>();
        keyEvent_ = KeyEvent::Create();
        keyEvent_->SetKeyCode(KEY_CODE_A);
        keyEvent_->SetDeviceId(DEVICE_ID);
        pointerEvent_ = PointerEvent::Create();
        pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
        mappingInfo_ = BuildKeyToTouchMappingInfo();
        windowInfo_ = BuildWindowInfoEntity();
        deviceInfo_.uniq = "notnull";
        deviceInfo_.deviceType = DeviceTypeEnum::GAME_KEY_BOARD;
        deviceInfo_.onlineTime = 0;
    }

    void TearDown() override
    {
        context_->ResetTempVariables();
    }

    static KeyToTouchMappingInfo BuildKeyToTouchMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.keyCode = KEY_CODE_A;
        info.mappingType = MappingTypeEnum::SINGE_KEY_TO_TOUCH;
        info.xValue = X_VALUE;
        info.yValue = Y_VALUE;
        return info;
    }

    static WindowInfoEntity BuildWindowInfoEntity()
    {
        WindowInfoEntity windowInfo;
        windowInfo.bundleName = "test";
        windowInfo.windowId = WINDOW_ID;
        windowInfo.maxWidth = MAX_WIDTH;
        windowInfo.maxHeight = MAX_HEIGHT;
        windowInfo.currentWidth = MAX_WIDTH;
        windowInfo.currentHeight = MAX_HEIGHT;
        windowInfo.xPosition = 0;
        windowInfo.yPosition = 0;
        windowInfo.isFullScreen = true;
        windowInfo.xCenter = MAX_WIDTH / 2;
        windowInfo.yCenter = MAX_HEIGHT / 2;
        windowInfo.displayId = 1;
        windowInfo.isPluginMode = false;
        return windowInfo;
    }

public:
    std::shared_ptr<TestKeyToTouchHandlerEx> handler_;
    std::shared_ptr<InputToTouchContext> context_;
    std::shared_ptr<MMI::KeyEvent> keyEvent_;
    std::shared_ptr<MMI::PointerEvent> pointerEvent_;
    KeyToTouchMappingInfo mappingInfo_;
    WindowInfoEntity windowInfo_;
    DeviceInfo deviceInfo_;
};

/**
 * @tc.name: IsMouseLeftButtonEvent_001
 * @tc.desc: when pointerEvent is mouse left button down event, return true
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsMouseLeftButtonEvent_001, TestSize.Level0)
{
    pointerEvent_->SetButtonId(MOUSE_LEFT_BUTTON_ID);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);

    bool result = BaseKeyToTouchHandler::IsMouseLeftButtonEvent(pointerEvent_);

    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsMouseLeftButtonEvent_002
 * @tc.desc: when pointerEvent is mouse left button up event, return true
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsMouseLeftButtonEvent_002, TestSize.Level0)
{
    pointerEvent_->SetButtonId(MOUSE_LEFT_BUTTON_ID);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);

    bool result = BaseKeyToTouchHandler::IsMouseLeftButtonEvent(pointerEvent_);

    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsMouseLeftButtonEvent_003
 * @tc.desc: when pointerEvent is not mouse source type, return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsMouseLeftButtonEvent_003, TestSize.Level0)
{
    pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent_->SetButtonId(MOUSE_LEFT_BUTTON_ID);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);

    bool result = BaseKeyToTouchHandler::IsMouseLeftButtonEvent(pointerEvent_);

    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsMouseLeftButtonEvent_004
 * @tc.desc: when pointerEvent is mouse right button event, return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsMouseLeftButtonEvent_004, TestSize.Level0)
{
    pointerEvent_->SetButtonId(MOUSE_RIGHT_BUTTON_ID);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);

    bool result = BaseKeyToTouchHandler::IsMouseLeftButtonEvent(pointerEvent_);

    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsMouseLeftButtonEvent_005
 * @tc.desc: when pointerEvent is mouse move event, return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsMouseLeftButtonEvent_005, TestSize.Level0)
{
    pointerEvent_->SetButtonId(MOUSE_LEFT_BUTTON_ID);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);

    bool result = BaseKeyToTouchHandler::IsMouseLeftButtonEvent(pointerEvent_);

    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsMouseRightButtonEvent_001
 * @tc.desc: when pointerEvent is mouse right button down event, return true
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsMouseRightButtonEvent_001, TestSize.Level0)
{
    pointerEvent_->SetButtonId(MOUSE_RIGHT_BUTTON_ID);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);

    bool result = BaseKeyToTouchHandler::IsMouseRightButtonEvent(pointerEvent_);

    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsMouseRightButtonEvent_002
 * @tc.desc: when pointerEvent is mouse right button up event, return true
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsMouseRightButtonEvent_002, TestSize.Level0)
{
    pointerEvent_->SetButtonId(MOUSE_RIGHT_BUTTON_ID);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);

    bool result = BaseKeyToTouchHandler::IsMouseRightButtonEvent(pointerEvent_);

    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsMouseRightButtonEvent_003
 * @tc.desc: when pointerEvent is not mouse source type, return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsMouseRightButtonEvent_003, TestSize.Level0)
{
    pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent_->SetButtonId(MOUSE_RIGHT_BUTTON_ID);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);

    bool result = BaseKeyToTouchHandler::IsMouseRightButtonEvent(pointerEvent_);

    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsMouseRightButtonEvent_004
 * @tc.desc: when pointerEvent is mouse left button event, return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsMouseRightButtonEvent_004, TestSize.Level0)
{
    pointerEvent_->SetButtonId(MOUSE_LEFT_BUTTON_ID);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);

    bool result = BaseKeyToTouchHandler::IsMouseRightButtonEvent(pointerEvent_);

    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsMouseMoveEvent_001
 * @tc.desc: when pointerEvent is mouse move event, return true
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsMouseMoveEvent_001, TestSize.Level0)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);

    bool result = BaseKeyToTouchHandler::IsMouseMoveEvent(pointerEvent_);

    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsMouseMoveEvent_002
 * @tc.desc: when pointerEvent is not mouse source type, return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsMouseMoveEvent_002, TestSize.Level0)
{
    pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);

    bool result = BaseKeyToTouchHandler::IsMouseMoveEvent(pointerEvent_);

    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsMouseMoveEvent_003
 * @tc.desc: when pointerEvent is mouse button down event, return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsMouseMoveEvent_003, TestSize.Level0)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);

    bool result = BaseKeyToTouchHandler::IsMouseMoveEvent(pointerEvent_);

    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsKeyUpEvent_001
 * @tc.desc: when keyEvent is KEY_ACTION_UP, return true
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsKeyUpEvent_001, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);

    bool result = BaseKeyToTouchHandler::IsKeyUpEvent(keyEvent_);

    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsKeyUpEvent_002
 * @tc.desc: when keyEvent is KEY_ACTION_CANCEL, return true
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsKeyUpEvent_002, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_CANCEL);

    bool result = BaseKeyToTouchHandler::IsKeyUpEvent(keyEvent_);

    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsKeyUpEvent_003
 * @tc.desc: when keyEvent is KEY_ACTION_DOWN, return false
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, IsKeyUpEvent_003, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);

    bool result = BaseKeyToTouchHandler::IsKeyUpEvent(keyEvent_);

    ASSERT_FALSE(result);
}

/**
 * @tc.name: HandleKeyEvent_001
 * @tc.desc: when keyEvent is null, HandleKeyEvent should return without error
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, HandleKeyEvent_001, TestSize.Level0)
{
    std::shared_ptr<MMI::KeyEvent> nullKeyEvent = nullptr;

    handler_->HandleKeyEvent(context_, nullKeyEvent, deviceInfo_, mappingInfo_);

    ASSERT_EQ(handler_->touchEntity_.pointerId, 0);
}

/**
 * @tc.name: HandleKeyEvent_002
 * @tc.desc: when keyEvent is KEY_ACTION_DOWN, HandleKeyDown should be called
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, HandleKeyEvent_002, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);

    handler_->HandleKeyEvent(context_, keyEvent_, deviceInfo_, mappingInfo_);

    ASSERT_EQ(handler_->touchEntity_.pointerId, 0);
}

/**
 * @tc.name: HandleKeyEvent_003
 * @tc.desc: when keyEvent is KEY_ACTION_UP, HandleKeyUp should be called
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, HandleKeyEvent_003, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);

    handler_->HandleKeyEvent(context_, keyEvent_, deviceInfo_, mappingInfo_);

    ASSERT_EQ(handler_->touchEntity_.pointerId, 0);
}

/**
 * @tc.name: HandleKeyEvent_004
 * @tc.desc: when keyEvent action is unknown, HandleKeyEvent should return without error
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, HandleKeyEvent_004, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UNKNOWN);

    handler_->HandleKeyEvent(context_, keyEvent_, deviceInfo_, mappingInfo_);

    ASSERT_EQ(handler_->touchEntity_.pointerId, 0);
}

/**
 * @tc.name: HandlePointerEvent_001
 * @tc.desc: HandlePointerEvent should be callable without error
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, HandlePointerEvent_001, TestSize.Level0)
{
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(handler_->touchEntity_.pointerId, 0);
}

/**
 * @tc.name: ExitCrosshairKeyStatus_001
 * @tc.desc: ExitCrosshairKeyStatus should be callable without error
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(BaseKeyToTouchHandlerTest, ExitCrosshairKeyStatus_001, TestSize.Level0)
{
    handler_->ExitCrosshairKeyStatus(context_);

    ASSERT_EQ(handler_->touchEntity_.pointerId, 0);
}

class PointerManagerTest : public testing::Test {
public:
    void SetUp() override
    {
        pointerManager_ = DelayedSingleton<PointerManager>::GetInstance();
    }

    void TearDown() override
    {
        pointerManager_->pointerIdCacheSet_.clear();
        pointerManager_.reset();
    }

public:
    std::shared_ptr<PointerManager> pointerManager_;
};

/**
 * @tc.name: ApplyPointerId_001
 * @tc.desc: ApplyPointerId should return a valid pointerId starting from 3
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(PointerManagerTest, ApplyPointerId_001, TestSize.Level0)
{
    int32_t pointerId = pointerManager_->ApplyPointerId();

    ASSERT_EQ(pointerId, POINTER_ID_START);
}

/**
 * @tc.name: ApplyPointerId_002
 * @tc.desc: ApplyPointerId should return different pointerIds for multiple calls
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(PointerManagerTest, ApplyPointerId_002, TestSize.Level0)
{
    int32_t pointerId1 = pointerManager_->ApplyPointerId();
    int32_t pointerId2 = pointerManager_->ApplyPointerId();
    int32_t pointerId3 = pointerManager_->ApplyPointerId();

    ASSERT_EQ(pointerId1, POINTER_ID_START);
    ASSERT_EQ(pointerId2, POINTER_ID_START + 1);
    ASSERT_EQ(pointerId3, POINTER_ID_START + 2);
}

/**
 * @tc.name: ReleasePointerId_001
 * @tc.desc: ReleasePointerId should release a pointerId successfully
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(PointerManagerTest, ReleasePointerId_001, TestSize.Level0)
{
    int32_t pointerId = pointerManager_->ApplyPointerId();

    pointerManager_->ReleasePointerId(pointerId);

    ASSERT_EQ(pointerId, POINTER_ID_START);
}

/**
 * @tc.name: ReleasePointerId_002
 * @tc.desc: After releasing a pointerId, ApplyPointerId should return the same pointerId
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(PointerManagerTest, ReleasePointerId_002, TestSize.Level0)
{
    int32_t pointerId1 = pointerManager_->ApplyPointerId();
    int32_t pointerId2 = pointerManager_->ApplyPointerId();

    pointerManager_->ReleasePointerId(pointerId1);
    int32_t pointerId3 = pointerManager_->ApplyPointerId();

    ASSERT_EQ(pointerId3, pointerId1);
}

class InputToTouchContextTest : public testing::Test {
public:
    void SetUp() override
    {
        context_ = std::make_shared<InputToTouchContext>();
        windowInfo_ = BuildWindowInfoEntity();
        deviceInfo_.uniq = "notnull";
        deviceInfo_.deviceType = DeviceTypeEnum::GAME_KEY_BOARD;
        deviceInfo_.onlineTime = 0;
    }

    void TearDown() override
    {
        context_->ResetTempVariables();
    }

    static KeyToTouchMappingInfo BuildSingleKeyMappingInfo(int32_t keyCode)
    {
        KeyToTouchMappingInfo info;
        info.keyCode = keyCode;
        info.mappingType = MappingTypeEnum::SINGE_KEY_TO_TOUCH;
        info.xValue = X_VALUE;
        info.yValue = Y_VALUE;
        return info;
    }

    static KeyToTouchMappingInfo BuildCombinationKeyMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.mappingType = MappingTypeEnum::COMBINATION_KEY_TO_TOUCH;
        info.combinationKeys = {KEY_CODE_A, KEY_CODE_B};
        return info;
    }

    static KeyToTouchMappingInfo BuildDpadKeyMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.mappingType = MappingTypeEnum::DPAD_KEY_TO_TOUCH;
        info.dpadKeyCodeEntity.up = KEY_CODE_A;
        info.dpadKeyCodeEntity.down = KEY_CODE_A + 1;
        info.dpadKeyCodeEntity.left = KEY_CODE_A + 2;
        info.dpadKeyCodeEntity.right = KEY_CODE_A + 3;
        return info;
    }

    static KeyToTouchMappingInfo BuildMouseRightKeyMappingInfo(MappingTypeEnum mappingType)
    {
        KeyToTouchMappingInfo info;
        info.mappingType = mappingType;
        return info;
    }

    static KeyToTouchMappingInfo BuildSkillKeyMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.keyCode = KEY_CODE_A;
        info.mappingType = MappingTypeEnum::SKILL_KEY_TO_TOUCH;
        info.radius = RADIUS;
        info.skillRange = 1;
        return info;
    }

    static KeyToTouchMappingInfo BuildCrosshairKeyMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.keyCode = KEY_CODE_A;
        info.mappingType = MappingTypeEnum::CROSSHAIR_KEY_TO_TOUCH;
        info.xStep = 10;
        info.yStep = 10;
        return info;
    }

    static KeyToTouchMappingInfo BuildObservationKeyMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.keyCode = KEY_CODE_A;
        info.mappingType = MappingTypeEnum::OBSERVATION_KEY_TO_TOUCH;
        return info;
    }

    static WindowInfoEntity BuildWindowInfoEntity()
    {
        WindowInfoEntity windowInfo;
        windowInfo.bundleName = "test";
        windowInfo.windowId = WINDOW_ID;
        windowInfo.maxWidth = MAX_WIDTH;
        windowInfo.maxHeight = MAX_HEIGHT;
        windowInfo.currentWidth = MAX_WIDTH;
        windowInfo.currentHeight = MAX_HEIGHT;
        windowInfo.xPosition = 0;
        windowInfo.yPosition = 0;
        windowInfo.isFullScreen = true;
        windowInfo.xCenter = MAX_WIDTH / 2;
        windowInfo.yCenter = MAX_HEIGHT / 2;
        windowInfo.displayId = 1;
        windowInfo.isPluginMode = false;
        return windowInfo;
    }

public:
    std::shared_ptr<InputToTouchContext> context_;
    WindowInfoEntity windowInfo_;
    DeviceInfo deviceInfo_;
};

/**
 * @tc.name: InputToTouchContext_Constructor_001
 * @tc.desc: Constructor should initialize context with SINGE_KEY_TO_TOUCH mapping
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, InputToTouchContext_Constructor_001, TestSize.Level0)
{
    std::vector<KeyToTouchMappingInfo> mappingInfos;
    mappingInfos.push_back(BuildSingleKeyMappingInfo(KEY_CODE_A));

    std::shared_ptr<InputToTouchContext> context =
        std::make_shared<InputToTouchContext>(DeviceTypeEnum::GAME_KEY_BOARD, windowInfo_, mappingInfos);

    ASSERT_FALSE(context == nullptr);
    ASSERT_EQ(context->singleKeyMappings.size(), 1);
    ASSERT_TRUE(context->singleKeyMappings.find(KEY_CODE_A) != context->singleKeyMappings.end());
    ASSERT_FALSE(context->isMonitorMouse);
}

/**
 * @tc.name: InputToTouchContext_Constructor_002
 * @tc.desc: Constructor should initialize context with COMBINATION_KEY_TO_TOUCH mapping
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, InputToTouchContext_Constructor_002, TestSize.Level0)
{
    std::vector<KeyToTouchMappingInfo> mappingInfos;
    mappingInfos.push_back(BuildCombinationKeyMappingInfo());

    std::shared_ptr<InputToTouchContext> context =
        std::make_shared<InputToTouchContext>(DeviceTypeEnum::GAME_KEY_BOARD, windowInfo_, mappingInfos);

    ASSERT_FALSE(context == nullptr);
    ASSERT_EQ(context->combinationKeyMappings.size(), 1);
    ASSERT_TRUE(context->combinationKeyMappings.find(KEY_CODE_B) != context->combinationKeyMappings.end());
    ASSERT_FALSE(context->isMonitorMouse);
}

/**
 * @tc.name: InputToTouchContext_Constructor_003
 * @tc.desc: Constructor should initialize context with DPAD_KEY_TO_TOUCH mapping
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, InputToTouchContext_Constructor_003, TestSize.Level0)
{
    std::vector<KeyToTouchMappingInfo> mappingInfos;
    mappingInfos.push_back(BuildDpadKeyMappingInfo());

    std::shared_ptr<InputToTouchContext> context =
        std::make_shared<InputToTouchContext>(DeviceTypeEnum::GAME_KEY_BOARD, windowInfo_, mappingInfos);

    ASSERT_FALSE(context == nullptr);
    ASSERT_EQ(context->singleKeyMappings.size(), 4);
    ASSERT_TRUE(context->singleKeyMappings.find(KEY_CODE_A) != context->singleKeyMappings.end());
    ASSERT_FALSE(context->isMonitorMouse);
}

/**
 * @tc.name: InputToTouchContext_Constructor_004
 * @tc.desc: Constructor should initialize context with MOUSE_RIGHT_KEY_WALKING_TO_TOUCH mapping
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, InputToTouchContext_Constructor_004, TestSize.Level0)
{
    std::vector<KeyToTouchMappingInfo> mappingInfos;
    mappingInfos.push_back(BuildMouseRightKeyMappingInfo(MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH));

    std::shared_ptr<InputToTouchContext> context =
        std::make_shared<InputToTouchContext>(DeviceTypeEnum::GAME_KEY_BOARD, windowInfo_, mappingInfos);

    ASSERT_FALSE(context == nullptr);
    ASSERT_EQ(context->mouseBtnKeyMappings.size(), 1);
    ASSERT_TRUE(context->mouseBtnKeyMappings.find(MOUSE_RIGHT_BUTTON_KEYCODE) != context->mouseBtnKeyMappings.end());
    ASSERT_TRUE(context->isMonitorMouse);
}

/**
 * @tc.name: InputToTouchContext_Constructor_005
 * @tc.desc: Constructor should initialize context with MOUSE_LEFT_FIRE_TO_TOUCH mapping
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, InputToTouchContext_Constructor_005, TestSize.Level0)
{
    std::vector<KeyToTouchMappingInfo> mappingInfos;
    mappingInfos.push_back(BuildMouseRightKeyMappingInfo(MappingTypeEnum::MOUSE_LEFT_FIRE_TO_TOUCH));

    std::shared_ptr<InputToTouchContext> context =
        std::make_shared<InputToTouchContext>(DeviceTypeEnum::GAME_KEY_BOARD, windowInfo_, mappingInfos);

    ASSERT_FALSE(context == nullptr);
    ASSERT_EQ(context->mouseBtnKeyMappings.size(), 1);
    ASSERT_TRUE(context->mouseBtnKeyMappings.find(MOUSE_LEFT_BUTTON_KEYCODE) != context->mouseBtnKeyMappings.end());
    ASSERT_TRUE(context->isMonitorMouse);
}

/**
 * @tc.name: InputToTouchContext_Constructor_006
 * @tc.desc: Constructor should initialize context with SKILL_KEY_TO_TOUCH mapping
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, InputToTouchContext_Constructor_006, TestSize.Level0)
{
    std::vector<KeyToTouchMappingInfo> mappingInfos;
    mappingInfos.push_back(BuildSkillKeyMappingInfo());

    std::shared_ptr<InputToTouchContext> context =
        std::make_shared<InputToTouchContext>(DeviceTypeEnum::GAME_KEY_BOARD, windowInfo_, mappingInfos);

    ASSERT_FALSE(context == nullptr);
    ASSERT_EQ(context->singleKeyMappings.size(), 1);
    ASSERT_TRUE(context->singleKeyMappings.find(KEY_CODE_A) != context->singleKeyMappings.end());
    ASSERT_TRUE(context->isMonitorMouse);
}

/**
 * @tc.name: InputToTouchContext_Constructor_007
 * @tc.desc: Constructor should initialize context with CROSSHAIR_KEY_TO_TOUCH mapping
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, InputToTouchContext_Constructor_007, TestSize.Level0)
{
    std::vector<KeyToTouchMappingInfo> mappingInfos;
    mappingInfos.push_back(BuildCrosshairKeyMappingInfo());

    std::shared_ptr<InputToTouchContext> context =
        std::make_shared<InputToTouchContext>(DeviceTypeEnum::GAME_KEY_BOARD, windowInfo_, mappingInfos);

    ASSERT_FALSE(context == nullptr);
    ASSERT_EQ(context->singleKeyMappings.size(), 1);
    ASSERT_TRUE(context->singleKeyMappings.find(KEY_CODE_A) != context->singleKeyMappings.end());
    ASSERT_TRUE(context->isMonitorMouse);
}

/**
 * @tc.name: InputToTouchContext_Constructor_008
 * @tc.desc: Constructor should initialize context with OBSERVATION_KEY_TO_TOUCH mapping
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, InputToTouchContext_Constructor_008, TestSize.Level0)
{
    std::vector<KeyToTouchMappingInfo> mappingInfos;
    mappingInfos.push_back(BuildObservationKeyMappingInfo());

    std::shared_ptr<InputToTouchContext> context =
        std::make_shared<InputToTouchContext>(DeviceTypeEnum::GAME_KEY_BOARD, windowInfo_, mappingInfos);

    ASSERT_FALSE(context == nullptr);
    ASSERT_EQ(context->singleKeyMappings.size(), 1);
    ASSERT_TRUE(context->singleKeyMappings.find(KEY_CODE_A) != context->singleKeyMappings.end());
    ASSERT_TRUE(context->isMonitorMouse);
}

/**
 * @tc.name: HasSingleKeyDown_001
 * @tc.desc: HasSingleKeyDown should return false when keyCode is not in currentSingleKeyMap
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, HasSingleKeyDown_001, TestSize.Level0)
{
    bool result = context_->HasSingleKeyDown(KEY_CODE_A);

    ASSERT_FALSE(result);
}

/**
 * @tc.name: HasSingleKeyDown_002
 * @tc.desc: HasSingleKeyDown should return true when keyCode is in currentSingleKeyMap
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, HasSingleKeyDown_002, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentSingleKeyInfo(BuildSingleKeyMappingInfo(KEY_CODE_A), pointerId);

    bool result = context_->HasSingleKeyDown(KEY_CODE_A);

    ASSERT_TRUE(result);
}

/**
 * @tc.name: SetCurrentSingleKeyInfo_001
 * @tc.desc: SetCurrentSingleKeyInfo should set mappingInfo and pointerId correctly
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, SetCurrentSingleKeyInfo_001, TestSize.Level0)
{
    KeyToTouchMappingInfo info = BuildSingleKeyMappingInfo(KEY_CODE_A);
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();

    context_->SetCurrentSingleKeyInfo(info, pointerId);

    ASSERT_EQ(context_->currentSingleKeyMap[KEY_CODE_A].keyCode, KEY_CODE_A);
    ASSERT_EQ(context_->pointerIdWithKeyCodeMap[KEY_CODE_A], pointerId);
}

/**
 * @tc.name: ResetCurrentSingleKeyInfo_001
 * @tc.desc: ResetCurrentSingleKeyInfo should remove keyCode from currentSingleKeyMap
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, ResetCurrentSingleKeyInfo_001, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentSingleKeyInfo(BuildSingleKeyMappingInfo(KEY_CODE_A), pointerId);

    context_->ResetCurrentSingleKeyInfo(KEY_CODE_A);

    ASSERT_FALSE(context_->HasSingleKeyDown(KEY_CODE_A));
}

/**
 * @tc.name: SetCurrentCombinationKey_001
 * @tc.desc: SetCurrentCombinationKey should set combinationKey and pointerId correctly
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, SetCurrentCombinationKey_001, TestSize.Level0)
{
    KeyToTouchMappingInfo info = BuildCombinationKeyMappingInfo();
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();

    context_->SetCurrentCombinationKey(info, pointerId);

    ASSERT_TRUE(context_->isCombinationKeyOperating);
    ASSERT_EQ(context_->currentCombinationKey.mappingType, MappingTypeEnum::COMBINATION_KEY_TO_TOUCH);
    ASSERT_EQ(context_->pointerIdWithKeyCodeMap[KEY_CODE_COMBINATION], pointerId);
}

/**
 * @tc.name: ResetCurrentCombinationKey_001
 * @tc.desc: ResetCurrentCombinationKey should reset combination key state
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, ResetCurrentCombinationKey_001, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentCombinationKey(BuildCombinationKeyMappingInfo(), pointerId);

    context_->ResetCurrentCombinationKey();

    ASSERT_FALSE(context_->isCombinationKeyOperating);
    ASSERT_EQ(context_->currentCombinationKey.combinationKeys.size(), 0);
}

/**
 * @tc.name: SetCurrentSkillKeyInfo_001
 * @tc.desc: SetCurrentSkillKeyInfo should set skillKeyInfo and pointerId correctly
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, SetCurrentSkillKeyInfo_001, TestSize.Level0)
{
    KeyToTouchMappingInfo info = BuildSkillKeyMappingInfo();
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();

    context_->SetCurrentSkillKeyInfo(info, pointerId);

    ASSERT_TRUE(context_->isSkillOperating);
    ASSERT_EQ(context_->currentSkillKeyInfo.mappingType, MappingTypeEnum::SKILL_KEY_TO_TOUCH);
    ASSERT_EQ(context_->pointerIdWithKeyCodeMap[KEY_CODE_SKILL], pointerId);
}

/**
 * @tc.name: ResetCurrentSkillKeyInfo_001
 * @tc.desc: ResetCurrentSkillKeyInfo should reset skill key state
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, ResetCurrentSkillKeyInfo_001, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentSkillKeyInfo(BuildSkillKeyMappingInfo(), pointerId);

    context_->ResetCurrentSkillKeyInfo();

    ASSERT_FALSE(context_->isSkillOperating);
    ASSERT_EQ(context_->currentSkillKeyInfo.skillRange, 0);
}

/**
 * @tc.name: SetCurrentObserving_001
 * @tc.desc: SetCurrentObserving should set observing and pointerId correctly
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, SetCurrentObserving_001, TestSize.Level0)
{
    KeyToTouchMappingInfo info = BuildObservationKeyMappingInfo();
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();

    context_->SetCurrentObserving(info, pointerId);

    ASSERT_TRUE(context_->isPerspectiveObserving);
    ASSERT_EQ(context_->currentPerspectiveObserving.mappingType, MappingTypeEnum::OBSERVATION_KEY_TO_TOUCH);
    ASSERT_EQ(context_->pointerIdWithKeyCodeMap[KEY_CODE_OBSERVATION], pointerId);
}

/**
 * @tc.name: ResetCurrentObserving_001
 * @tc.desc: ResetCurrentObserving should reset observing state
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, ResetCurrentObserving_001, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentObserving(BuildObservationKeyMappingInfo(), pointerId);

    context_->ResetCurrentObserving();

    ASSERT_FALSE(context_->isPerspectiveObserving);
}

/**
 * @tc.name: SetCurrentCrosshairInfo_001
 * @tc.desc: SetCurrentCrosshairInfo should set crosshairInfo and pointerId correctly
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, SetCurrentCrosshairInfo_001, TestSize.Level0)
{
    KeyToTouchMappingInfo info = BuildCrosshairKeyMappingInfo();
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();

    context_->SetCurrentCrosshairInfo(info, pointerId);

    ASSERT_TRUE(context_->isCrosshairMode);
    ASSERT_FALSE(context_->isEnterCrosshairInfo);
    ASSERT_EQ(context_->currentCrosshairInfo.mappingType, MappingTypeEnum::CROSSHAIR_KEY_TO_TOUCH);
    ASSERT_EQ(context_->pointerIdWithKeyCodeMap[KEY_CODE_CROSSHAIR], pointerId);
}

/**
 * @tc.name: ResetCurrentCrosshairInfo_001
 * @tc.desc: ResetCurrentCrosshairInfo should reset crosshair state
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, ResetCurrentCrosshairInfo_001, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentCrosshairInfo(BuildCrosshairKeyMappingInfo(), pointerId);

    context_->ResetCurrentCrosshairInfo();

    ASSERT_FALSE(context_->isCrosshairMode);
    ASSERT_FALSE(context_->isEnterCrosshairInfo);
}

/**
 * @tc.name: SetCurrentWalking_001
 * @tc.desc: SetCurrentWalking should set walking and pointerId correctly
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, SetCurrentWalking_001, TestSize.Level0)
{
    KeyToTouchMappingInfo info = BuildMouseRightKeyMappingInfo(MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH);
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();

    context_->SetCurrentWalking(info, pointerId);

    ASSERT_TRUE(context_->isWalking);
    ASSERT_EQ(context_->currentWalking.mappingType, MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH);
    ASSERT_EQ(context_->pointerIdWithKeyCodeMap[KEY_CODE_WALK], pointerId);
}

/**
 * @tc.name: ResetCurrentWalking_001
 * @tc.desc: ResetCurrentWalking should reset walking state
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, ResetCurrentWalking_001, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(
        BuildMouseRightKeyMappingInfo(MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH), pointerId);

    context_->ResetCurrentWalking();

    ASSERT_FALSE(context_->isWalking);
}

/**
 * @tc.name: IsMouseRightWalking_001
 * @tc.desc: IsMouseRightWalking should return true when walking with mouse right button
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, IsMouseRightWalking_001, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(
        BuildMouseRightKeyMappingInfo(MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH), pointerId);

    bool result = context_->IsMouseRightWalking();

    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsMouseRightWalking_002
 * @tc.desc: IsMouseRightWalking should return false when not walking
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, IsMouseRightWalking_002, TestSize.Level0)
{
    bool result = context_->IsMouseRightWalking();

    ASSERT_FALSE(result);
}

/**
 * @tc.name: SetCurrentMouseLeftClick_001
 * @tc.desc: SetCurrentMouseLeftClick should set mouse left click state
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, SetCurrentMouseLeftClick_001, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();

    context_->SetCurrentMouseLeftClick(pointerId);

    ASSERT_TRUE(context_->isMouseLeftFireOperating);
    ASSERT_EQ(context_->pointerIdWithKeyCodeMap[KEY_CODE_MOUSE_LEFT], pointerId);
}

/**
 * @tc.name: ResetCurrentMouseLeftClick_001
 * @tc.desc: ResetCurrentMouseLeftClick should reset mouse left click state
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, ResetCurrentMouseLeftClick_001, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentMouseLeftClick(pointerId);

    context_->ResetCurrentMouseLeftClick();

    ASSERT_FALSE(context_->isMouseLeftFireOperating);
}

/**
 * @tc.name: SetCurrentMouseRightClick_001
 * @tc.desc: SetCurrentMouseRightClick should set mouse right click state
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, SetCurrentMouseRightClick_001, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();

    context_->SetCurrentMouseRightClick(pointerId);

    ASSERT_TRUE(context_->isMouseRightClickOperating);
    ASSERT_EQ(context_->pointerIdWithKeyCodeMap[KEY_CODE_MOUSE_RIGHT], pointerId);
}

/**
 * @tc.name: ResetCurrentMouseRightClick_001
 * @tc.desc: ResetCurrentMouseRightClick should reset mouse right click state
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, ResetCurrentMouseRightClick_001, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentMouseRightClick(pointerId);

    context_->ResetCurrentMouseRightClick();

    ASSERT_FALSE(context_->isMouseRightClickOperating);
}

/**
 * @tc.name: ResetTempVariables_001
 * @tc.desc: ResetTempVariables should reset all temporary variables
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, ResetTempVariables_001, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentSingleKeyInfo(BuildSingleKeyMappingInfo(KEY_CODE_A), pointerId);
    context_->SetCurrentCombinationKey(BuildCombinationKeyMappingInfo(), pointerId);
    context_->SetCurrentSkillKeyInfo(BuildSkillKeyMappingInfo(), pointerId);
    context_->SetCurrentObserving(BuildObservationKeyMappingInfo(), pointerId);
    context_->SetCurrentCrosshairInfo(BuildCrosshairKeyMappingInfo(), pointerId);
    context_->SetCurrentWalking(
        BuildMouseRightKeyMappingInfo(MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH), pointerId);
    context_->SetCurrentMouseLeftClick(pointerId);
    context_->SetCurrentMouseRightClick(pointerId);

    context_->ResetTempVariables();

    ASSERT_EQ(context_->currentSingleKeyMap.size(), 0);
    ASSERT_EQ(context_->pointerIdWithKeyCodeMap.size(), 0);
    ASSERT_EQ(context_->pointerItems.size(), 0);
    ASSERT_FALSE(context_->isCombinationKeyOperating);
    ASSERT_FALSE(context_->isSkillOperating);
    ASSERT_FALSE(context_->isPerspectiveObserving);
    ASSERT_FALSE(context_->isCrosshairMode);
    ASSERT_FALSE(context_->isWalking);
    ASSERT_FALSE(context_->isMouseLeftFireOperating);
    ASSERT_FALSE(context_->isMouseRightClickOperating);
}

/**
 * @tc.name: GetPointerIdByKeyCode_001
 * @tc.desc: GetPointerIdByKeyCode should return false when keyCode is not in map
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, GetPointerIdByKeyCode_001, TestSize.Level0)
{
    std::pair<bool, int32_t> result = context_->GetPointerIdByKeyCode(KEY_CODE_A);

    ASSERT_FALSE(result.first);
}

/**
 * @tc.name: GetPointerIdByKeyCode_002
 * @tc.desc: GetPointerIdByKeyCode should return true and pointerId when keyCode is in map
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, GetPointerIdByKeyCode_002, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentSingleKeyInfo(BuildSingleKeyMappingInfo(KEY_CODE_A), pointerId);

    std::pair<bool, int32_t> result = context_->GetPointerIdByKeyCode(KEY_CODE_A);

    ASSERT_TRUE(result.first);
    ASSERT_EQ(result.second, pointerId);
}

/**
 * @tc.name: GetEventId_001
 * @tc.desc: GetEventId should return max int32 value
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, GetEventId_001, TestSize.Level0)
{
    int32_t eventId = context_->GetEventId();

    ASSERT_EQ(eventId, std::numeric_limits<int32_t>::max());
}

/**
 * @tc.name: CheckPointerSendInterval_001
 * @tc.desc: CheckPointerSendInterval should return without error when pointerItems is empty
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, CheckPointerSendInterval_001, TestSize.Level0)
{
    context_->CheckPointerSendInterval();

    ASSERT_EQ(context_->pointerItems.size(), 0);
}

/**
 * @tc.name: WindowInfoEntity_ParseRect_001
 * @tc.desc: ParseRect should parse Rosen::Rect correctly
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, WindowInfoEntity_ParseRect_001, TestSize.Level0)
{
    WindowInfoEntity windowInfo;
    Rosen::Rect rect;
    rect.posX_ = 100;
    rect.posY_ = 200;
    rect.width_ = 300;
    rect.height_ = 400;

    windowInfo.ParseRect(rect);

    ASSERT_EQ(windowInfo.xPosition, 100);
    ASSERT_EQ(windowInfo.yPosition, 200);
    ASSERT_EQ(windowInfo.currentWidth, 300);
    ASSERT_EQ(windowInfo.currentHeight, 400);
    ASSERT_EQ(windowInfo.xCenter, 150);
    ASSERT_EQ(windowInfo.yCenter, 200);
}

/**
 * @tc.name: WindowInfoEntity_ToString_001
 * @tc.desc: ToString should return correct string representation
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(InputToTouchContextTest, WindowInfoEntity_ToString_001, TestSize.Level0)
{
    WindowInfoEntity windowInfo;
    windowInfo.windowId = 1;
    windowInfo.maxWidth = 100;
    windowInfo.maxHeight = 200;
    windowInfo.currentWidth = 100;
    windowInfo.currentHeight = 200;
    windowInfo.xPosition = 10;
    windowInfo.yPosition = 20;
    windowInfo.isFullScreen = true;
    windowInfo.xCenter = 50;
    windowInfo.yCenter = 100;

    std::string result = windowInfo.ToString();

    ASSERT_NE(result.find("windowId:1"), std::string::npos);
    ASSERT_NE(result.find("maxWidth:100"), std::string::npos);
    ASSERT_NE(result.find("maxHeight:200"), std::string::npos);
}
}
}