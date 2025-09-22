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
#define protected public

#include "keyboard_observation_to_touch_handler.h"
#include "key_to_touch_handler.h"

#define protected public
#undef private

#include "refbase.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
namespace {
const int32_t X_STEP = 1;
const int32_t Y_STEP = 2;
const int32_t DEVICE_ID = 11;
const int32_t X_VALUE = 2045;
const int32_t Y_VALUE = 1003;
const int32_t MAX_WIDTH = 2720;
const int32_t MAX_HEIGHT = 1260;
const int32_t KEY_CODE_UP = 2301;
const int32_t KEY_CODE_DOWN = 2302;
const int32_t KEY_CODE_LEFT = 2303;
const int32_t KEY_CODE_RIGHT = 2304;
}

class KeyboardObservationToTouchHandlerExt : public KeyboardObservationToTouchHandler {
public:
    void BuildAndSendPointerEvent(std::shared_ptr<InputToTouchContext> &context_,
                                  const TouchEntity &touchEntity) override
    {
        BaseKeyToTouchHandler::BuildAndSendPointerEvent(context_, touchEntity);
        touchEntity_ = touchEntity;
    }

    void HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                       const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                       const KeyToTouchMappingInfo &mappingInfo, const DeviceInfo &deviceInfo) override
    {
        KeyboardObservationToTouchHandler::HandleKeyDown(context, keyEvent, mappingInfo, deviceInfo);
    }

    void HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                     const std::shared_ptr<MMI::KeyEvent> &keyEvent, const DeviceInfo &deviceInfo) override
    {
        KeyboardObservationToTouchHandler::HandleKeyUp(context, keyEvent, deviceInfo);
    }

public:
    TouchEntity touchEntity_;
};

class KeyboardObservationToTouchHandlerTest : public testing::Test {
public:
    void SetUp()
    {
        DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->StopTask();
        handler_ = std::make_shared<KeyboardObservationToTouchHandlerExt>();
        context_ = std::make_shared<InputToTouchContext>();
        context_->windowInfoEntity.maxWidth = MAX_WIDTH;
        context_->windowInfoEntity.maxHeight = MAX_HEIGHT;
        keyEvent_ = KeyEvent::Create();
        keyEvent_->SetKeyCode(KEY_CODE_UP);
        keyEvent_->SetDeviceId(DEVICE_ID);
        mappingInfo_ = BuildKeyToTouchMappingInfo();
        deviceInfo_.onlineTime = 1;
    }

    KeyToTouchMappingInfo BuildKeyToTouchMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.dpadKeyCodeEntity.up = KEY_CODE_UP;
        info.dpadKeyCodeEntity.down = KEY_CODE_DOWN;
        info.dpadKeyCodeEntity.left = KEY_CODE_LEFT;
        info.dpadKeyCodeEntity.right = KEY_CODE_RIGHT;
        info.mappingType = MappingTypeEnum::KEY_BOARD_OBSERVATION_TO_TOUCH;
        info.xValue = X_VALUE;
        info.xStep = X_STEP;
        info.yValue = Y_VALUE;
        info.yStep = Y_STEP;
        return info;
    }

    KeyEvent::KeyItem BuildKeyItem(int32_t keycode, bool isPressed)
    {
        KeyEvent::KeyItem keyItem;
        keyItem.SetKeyCode(keycode);
        keyItem.SetPressed(isPressed);
        keyItem.SetDownTime(keycode);
        keyItem.SetDeviceId(DEVICE_ID);
        return keyItem;
    }

    void TearDown()
    {
        DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->StopTask();
    }

public:
    std::shared_ptr<KeyboardObservationToTouchHandlerExt> handler_;
    std::shared_ptr<InputToTouchContext> context_;
    std::shared_ptr<MMI::KeyEvent> keyEvent_;
    KeyToTouchMappingInfo mappingInfo_;
    DeviceInfo deviceInfo_;
};

/**
 * @tc.name: HandleKeyDown_001
 * @tc.desc: when key is down and context_->isPerspectiveObserving is false, can send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_001, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_UP, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->isPerspectiveObserving);
    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[OBSERVATION_POINT_ID];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE);
    ASSERT_EQ(context_->currentPerspectiveObserving.mappingType, mappingInfo_.mappingType);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(handler_->touchEntity_.pointerId, OBSERVATION_POINT_ID);

    ASSERT_TRUE(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->taskIsStarting_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 1);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_UP);
}

/**
 * @tc.name: HandleKeyDown_002
 * @tc.desc: when key is down and keycode is up and context_->isPerspectiveObserving is true,
 * and have the pressed keycodes of up, it will move to the up
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_002, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(KEY_CODE_UP);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_UP, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    // move to the edge of up
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 1);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_UP);
}

/**
 * @tc.name: HandleKeyDown_003
 * @tc.desc: when key is down and keycode is down and context_->isPerspectiveObserving is true,
 * and have the pressed keycodes of down, it will move to the down
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_003, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(KEY_CODE_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_DOWN, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    // move to the edge of down
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 1);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_DOWN);
}

/**
 * @tc.name: HandleKeyDown_004
 * @tc.desc: when key is down and keycode is left and context_->isPerspectiveObserving is true,
 * and have the pressed keycodes of left, it will move to the left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_004, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(KEY_CODE_LEFT);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_LEFT, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    // move to the edge of left
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_LEFT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_LEFT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 1);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_LEFT);
}

/**
 * @tc.name: HandleKeyDown_005
 * @tc.desc: when key is down and keycode is right and context_->isPerspectiveObserving is true,
 * and have the pressed keycodes of left, it will move to the right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_005, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(KEY_CODE_RIGHT);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_RIGHT, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    // move to the edge of right
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_RIGHT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_RIGHT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 1);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_RIGHT);
}

/**
 * @tc.name: HandleKeyDown_006
 * @tc.desc: when key is down and keycode is left and context_->isPerspectiveObserving is true,
 * and have the pressed keycodes of up and left and down,
 * and the downTime of down is more then the downTime of up
 * then it will move to the up and left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_006, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(KEY_CODE_LEFT);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_DOWN, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_LEFT, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_RIGHT, false));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_UP, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_LEFT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_LEFT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 3);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[1].keyCode,
              KEY_CODE_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[2].keyCode,
              KEY_CODE_LEFT);
}

/**
 * @tc.name: HandleKeyDown_007
 * @tc.desc: when key is down and keycode is left and context_->isPerspectiveObserving is true,
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of down is less then the downTime of up
 * then it will move to the down and left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_007, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(KEY_CODE_LEFT);
    KeyEvent::KeyItem keyItem = BuildKeyItem(KEY_CODE_DOWN, true);
    keyItem.SetDownTime(1000);
    keyEvent_->AddKeyItem(keyItem);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_LEFT, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_RIGHT, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_UP, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_LEFT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_LEFT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 4);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[1].keyCode,
              KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[2].keyCode,
              KEY_CODE_LEFT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[3].keyCode,
              KEY_CODE_RIGHT);
}

/**
 * @tc.name: HandleKeyDown_008
 * @tc.desc: when key is down and keycode is right and context_->isPerspectiveObserving is true,
 * and have the pressed keycodes of  up and down and left and right,
 * and the downTime of down is more then the downTime of up
 * then it will move to the up and right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_008, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(KEY_CODE_RIGHT);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_DOWN, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_LEFT, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_RIGHT, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_UP, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_RIGHT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_RIGHT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 4);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[1].keyCode,
              KEY_CODE_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[2].keyCode,
              KEY_CODE_LEFT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[3].keyCode,
              KEY_CODE_RIGHT);
}

/**
 * @tc.name: HandleKeyDown_009
 * @tc.desc: when key is down and keycode is right and context_->isPerspectiveObserving is true,
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of down is more then the downTime of up
 * then it will move to the down and right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_009, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(KEY_CODE_RIGHT);
    KeyEvent::KeyItem keyItem = BuildKeyItem(KEY_CODE_DOWN, true);
    keyItem.SetDownTime(1000);
    keyEvent_->AddKeyItem(keyItem);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_LEFT, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_RIGHT, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_UP, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);
    PointerEvent::PointerItem pointerItem = context_->pointerItems[OBSERVATION_POINT_ID];

    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_RIGHT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_RIGHT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 4);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[1].keyCode,
              KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[2].keyCode,
              KEY_CODE_LEFT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[3].keyCode,
              KEY_CODE_RIGHT);
}

/**
 * @tc.name: HandleKeyDown_010
 * @tc.desc: when key is down and keycode is up and context_->isPerspectiveObserving is true,
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of left is less then the downTime of right
 * then it will move to the up and left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_010, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(KEY_CODE_UP);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_DOWN, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_LEFT, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_RIGHT, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_UP, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 4);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[1].keyCode,
              KEY_CODE_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[2].keyCode,
              KEY_CODE_LEFT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[3].keyCode,
              KEY_CODE_RIGHT);
}

/**
 * @tc.name: HandleKeyDown_011
 * @tc.desc: when key is down and keycode is up and context_->isPerspectiveObserving is true,
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of left is more then the downTime of right
 * then it will move to the up and right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_011, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(KEY_CODE_UP);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_DOWN, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_LEFT, true));
    KeyEvent::KeyItem keyItem = BuildKeyItem(KEY_CODE_RIGHT, true);
    keyItem.SetDownTime(1000);
    keyEvent_->AddKeyItem(keyItem);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_UP, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 4);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_RIGHT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[1].keyCode,
              KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[2].keyCode,
              KEY_CODE_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[3].keyCode,
              KEY_CODE_LEFT);
}

/**
 * @tc.name: HandleKeyDown_012
 * @tc.desc: when key is down and keycode is down and context_->isPerspectiveObserving is true,
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of left is less then the downTime of right
 * then it will move to the down and left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_012, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(KEY_CODE_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_DOWN, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_LEFT, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_RIGHT, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_UP, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 4);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[1].keyCode,
              KEY_CODE_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[2].keyCode,
              KEY_CODE_LEFT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[3].keyCode,
              KEY_CODE_RIGHT);
}

/**
 * @tc.name: HandleKeyDown_013
 * @tc.desc: when key is down and keycode is down and context_->isPerspectiveObserving is true,
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of left is more then the downTime of right
 * then it will move to the down and right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyDown_013, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(KEY_CODE_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_DOWN, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_LEFT, true));
    KeyEvent::KeyItem keyItem = BuildKeyItem(KEY_CODE_RIGHT, true);
    keyItem.SetDownTime(1000);
    keyEvent_->AddKeyItem(keyItem);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_UP, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 4);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_RIGHT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[1].keyCode,
              KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[2].keyCode,
              KEY_CODE_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[3].keyCode,
              KEY_CODE_LEFT);
}

/**
 * @tc.name: HandleKeyUp_001
 * @tc.desc: when key is up and context_->isPerspectiveObserving is true,
 * and there is not any pressed key. can send up-touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyUp_001, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_FALSE(context_->isPerspectiveObserving);
    ASSERT_EQ(context_->currentPerspectiveObserving.mappingType, 0);
    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) == context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchEntity_.pointerId, OBSERVATION_POINT_ID);
    ASSERT_FALSE(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->taskIsStarting_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, 0);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_,
              DPAD_KEYTYPE_UNKNOWN);
    ASSERT_TRUE(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.empty());
}

/**
 * @tc.name: HandleKeyUp_002
 * @tc.desc: when key is up and context_->isPerspectiveObserving is false, cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyUp_002, TestSize.Level1)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);
    context_->isPerspectiveObserving = false;
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_TRUE(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->taskIsStarting_);
}

/**
 * @tc.name: HandleKeyUp_003
 * @tc.desc: when key is up and context_->isPerspectiveObserving is true,
 * and currentPerspectiveObserving.mappingType is not KEY_BOARD_OBSERVATION_TO_TOUCH,
 * cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyUp_003, TestSize.Level1)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);
    context_->currentPerspectiveObserving.mappingType = OBSERVATION_KEY_TO_TOUCH;
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) != context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_TRUE(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->taskIsStarting_);
}

/**
 * @tc.name: HandleKeyUp_004
 * @tc.desc: when key is up and keycode is left and context_->isPerspectiveObserving is true
 * and other dpad's key in keyItems,
 * and have the pressed keycodes of up and down and right,
 * and the downTime of the right is largest and the the downTime of up is less than the downTime of down
 * then it will move to the up and right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, HandleKeyUp_004, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_CODE_LEFT);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_DOWN, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_UP, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_LEFT, false));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE_RIGHT, true));
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_, KEY_CODE_RIGHT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_, context_);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_, DPAD_RIGHT);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.size(), 3);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[0].keyCode,
              KEY_CODE_UP);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[1].keyCode,
              KEY_CODE_DOWN);
    ASSERT_EQ(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_[2].keyCode,
              KEY_CODE_RIGHT);
    ASSERT_TRUE(DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->taskIsStarting_);
}

static DpadKeyItem BuildDpadKeyItem(int32_t keyCode, DpadKeyTypeEnum keyTypeEnum)
{
    DpadKeyItem dpadKeyItem;
    dpadKeyItem.keyCode = keyCode;
    dpadKeyItem.keyTypeEnum = keyTypeEnum;
    return dpadKeyItem;
}

/**
 * @tc.name: ComputeAndSendMovePointer_001
 * @tc.desc: param is invalid, no send pointer
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, ComputeAndSendMovePointer_001, TestSize.Level0)
{
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) == context_->pointerItems.end());

    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_ = context_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) == context_->pointerItems.end());

    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_DOWN, DPAD_DOWN));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) == context_->pointerItems.end());

    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_ = KEY_CODE_DOWN;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) == context_->pointerItems.end());

    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_ = DPAD_DOWN;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) == context_->pointerItems.end());
}

static PointerEvent::PointerItem BuildPointerItem()
{
    PointerEvent::PointerItem pointerItem;
    pointerItem.SetWindowX(X_VALUE);
    pointerItem.SetWindowY(Y_VALUE);
    return pointerItem;
}

/**
 * @tc.name: ComputeAndSendMovePointer_002
 * @tc.desc: when keycode is down, move to the down
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, ComputeAndSendMovePointer_002, TestSize.Level0)
{
    context_->currentPerspectiveObserving = mappingInfo_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_ = context_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_DOWN, DPAD_DOWN));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_ = KEY_CODE_DOWN;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_ = DPAD_DOWN;
    context_->pointerItems[OBSERVATION_POINT_ID] = BuildPointerItem();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();

    PointerEvent::PointerItem pointerItem = context_->pointerItems[OBSERVATION_POINT_ID];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE + Y_STEP);
}

/**
 * @tc.name: ComputeAndSendMovePointer_003
 * @tc.desc: when keycode is up, move to the up
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, ComputeAndSendMovePointer_003, TestSize.Level0)
{
    context_->currentPerspectiveObserving = mappingInfo_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_ = context_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_UP, DPAD_UP));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_ = KEY_CODE_UP;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_ = DPAD_UP;
    context_->pointerItems[OBSERVATION_POINT_ID] = BuildPointerItem();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();

    PointerEvent::PointerItem pointerItem = context_->pointerItems[OBSERVATION_POINT_ID];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE - Y_STEP);
}

/**
 * @tc.name: ComputeAndSendMovePointer_004
 * @tc.desc: when keycode is left, move to the left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, ComputeAndSendMovePointer_004, TestSize.Level0)
{
    context_->currentPerspectiveObserving = mappingInfo_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_ = context_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_LEFT, DPAD_LEFT));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_ = KEY_CODE_LEFT;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_ = DPAD_LEFT;
    context_->pointerItems[OBSERVATION_POINT_ID] = BuildPointerItem();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();

    PointerEvent::PointerItem pointerItem = context_->pointerItems[OBSERVATION_POINT_ID];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE - X_STEP);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE);
}

/**
 * @tc.name: ComputeAndSendMovePointer_005
 * @tc.desc: when keycode is right, move to the right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, ComputeAndSendMovePointer_005, TestSize.Level0)
{
    context_->currentPerspectiveObserving = mappingInfo_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_ = context_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_RIGHT, DPAD_RIGHT));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_ = KEY_CODE_RIGHT;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_ = DPAD_RIGHT;
    context_->pointerItems[OBSERVATION_POINT_ID] = BuildPointerItem();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();

    PointerEvent::PointerItem pointerItem = context_->pointerItems[OBSERVATION_POINT_ID];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE + X_STEP);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE);
}

/**
 * @tc.name: ComputeAndSendMovePointer_006
 * @tc.desc: when keycode is down and right, move to the down and right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, ComputeAndSendMovePointer_006, TestSize.Level0)
{
    context_->currentPerspectiveObserving = mappingInfo_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_ = context_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_RIGHT, DPAD_RIGHT));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_DOWN, DPAD_DOWN));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_ = KEY_CODE_RIGHT;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_ = DPAD_RIGHT;
    context_->pointerItems[OBSERVATION_POINT_ID] = BuildPointerItem();
    context_->pointerItems[OBSERVATION_POINT_ID].SetWindowX(MAX_WIDTH);
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();

    PointerEvent::PointerItem pointerItem = context_->pointerItems[OBSERVATION_POINT_ID];
    ASSERT_EQ(pointerItem.GetWindowX(), MAX_WIDTH);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE + Y_STEP);
}

/**
 * @tc.name: ComputeAndSendMovePointer_007
 * @tc.desc: when keycode is up and right, move to the up and right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, ComputeAndSendMovePointer_007, TestSize.Level0)
{
    context_->currentPerspectiveObserving = mappingInfo_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_ = context_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_RIGHT, DPAD_RIGHT));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_UP, DPAD_UP));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_ = KEY_CODE_RIGHT;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_ = DPAD_RIGHT;
    context_->pointerItems[OBSERVATION_POINT_ID] = BuildPointerItem();
    context_->pointerItems[OBSERVATION_POINT_ID].SetWindowY(0);
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();

    PointerEvent::PointerItem pointerItem = context_->pointerItems[OBSERVATION_POINT_ID];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE + X_STEP);
    ASSERT_EQ(pointerItem.GetWindowY(), 0);
}

/**
 * @tc.name: ComputeAndSendMovePointer_008
 * @tc.desc: when keycode is down and left, move to the down and left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, ComputeAndSendMovePointer_008, TestSize.Level0)
{
    context_->currentPerspectiveObserving = mappingInfo_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_ = context_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_LEFT, DPAD_LEFT));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_DOWN, DPAD_DOWN));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_ = KEY_CODE_LEFT;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_ = DPAD_LEFT;
    context_->pointerItems[OBSERVATION_POINT_ID] = BuildPointerItem();
    context_->pointerItems[OBSERVATION_POINT_ID].SetWindowX(0);
    context_->pointerItems[OBSERVATION_POINT_ID].SetWindowY(MAX_HEIGHT);
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();

    PointerEvent::PointerItem pointerItem = context_->pointerItems[OBSERVATION_POINT_ID];
    ASSERT_EQ(pointerItem.GetWindowX(), 0);
    ASSERT_EQ(pointerItem.GetWindowY(), MAX_HEIGHT);
}

/**
 * @tc.name: ComputeAndSendMovePointer_009
 * @tc.desc: when keycode is up and left, move to the up and left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyboardObservationToTouchHandlerTest, ComputeAndSendMovePointer_009, TestSize.Level0)
{
    context_->currentPerspectiveObserving = mappingInfo_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->context_ = context_;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_LEFT, DPAD_LEFT));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->dpadKeys_.push_back(
        BuildDpadKeyItem(KEY_CODE_UP, DPAD_UP));
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentKeyCode_ = KEY_CODE_LEFT;
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->currentDpadKeyType_ = DPAD_LEFT;
    context_->pointerItems[OBSERVATION_POINT_ID] = BuildPointerItem();
    DelayedSingleton<KeyboardObservationToTouchHandlerTask>::GetInstance()->ComputeAndSendMovePointer();

    PointerEvent::PointerItem pointerItem = context_->pointerItems[OBSERVATION_POINT_ID];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE - X_STEP);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE - Y_STEP);
}
}
}