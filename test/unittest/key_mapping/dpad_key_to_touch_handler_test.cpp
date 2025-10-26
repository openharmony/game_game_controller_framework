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
#include "dpad_key_to_touch_handler.h"
#include "refbase.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
namespace {
const int32_t X_VALUE = 539;
const int32_t Y_VALUE = 1001;
const int32_t DEVICE_ID = 11;
const int32_t OTHER_DEVICE_ID = 12;
const int32_t RADIUS = 230;
const int32_t KEY_UP_CODE = 2301;
const int32_t KEY_DOWN_CODE = 2302;
const int32_t KEY_LEFT_CODE = 2303;
const int32_t KEY_RIGHT_CODE = 2304;
const int32_t KEY_CODE = 2305;
}
class DpadKeyToTouchHandlerEx : public DpadKeyToTouchHandler {
public:
    void BuildAndSendPointerEvent(std::shared_ptr<InputToTouchContext> &context_,
                                  const TouchEntity &touchEntity) override
    {
        hasTouchEvent_ = true;
        BaseKeyToTouchHandler::BuildAndSendPointerEvent(context_, touchEntity);
        if (touchEntity.pointerAction == PointerEvent::POINTER_ACTION_DOWN) {
            touchDownEntity_ = touchEntity;
        }
        if (touchEntity.pointerAction == PointerEvent::POINTER_ACTION_MOVE) {
            touchMoveEntity_ = touchEntity;
        }
        if (touchEntity.pointerAction == PointerEvent::POINTER_ACTION_UP) {
            touchUpEntity_ = touchEntity;
        }
    }

    void HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                       const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                       const KeyToTouchMappingInfo &mappingInfo, const DeviceInfo &deviceInfo) override
    {
        DpadKeyToTouchHandler::HandleKeyDown(context, keyEvent, mappingInfo, deviceInfo);
    }

    void HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                     const std::shared_ptr<MMI::KeyEvent> &keyEvent, const DeviceInfo &deviceInfo) override
    {
        DpadKeyToTouchHandler::HandleKeyUp(context, keyEvent, deviceInfo);
    }

public:
    TouchEntity touchDownEntity_;
    TouchEntity touchMoveEntity_;
    TouchEntity touchUpEntity_;
    bool hasTouchEvent_{false};
};

class DpadKeyToTouchHandlerTest : public testing::Test {
public:
    void SetUp() override
    {
        handler_ = std::make_shared<DpadKeyToTouchHandlerEx>();
        context_ = std::make_shared<InputToTouchContext>();
        keyEvent_ = KeyEvent::Create();
        keyEvent_->SetDeviceId(DEVICE_ID);
        keyEvent_->SetKeyCode(KEY_UP_CODE);
        mappingInfo_ = BuildKeyToTouchMappingInfo();
        deviceInfo_.onlineTime = 1;
    }

    void TearDown() override
    {
        context_->ResetCurrentWalking();
    }

    static KeyToTouchMappingInfo BuildKeyToTouchMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.dpadKeyCodeEntity.up = KEY_UP_CODE;
        info.dpadKeyCodeEntity.down = KEY_DOWN_CODE;
        info.dpadKeyCodeEntity.left = KEY_LEFT_CODE;
        info.dpadKeyCodeEntity.right = KEY_RIGHT_CODE;
        info.mappingType = MappingTypeEnum::DPAD_KEY_TO_TOUCH;
        info.xValue = X_VALUE;
        info.yValue = Y_VALUE;
        info.radius = RADIUS;
        return info;
    }

    static KeyEvent::KeyItem BuildKeyItem(int32_t keycode, bool isPressed)
    {
        KeyEvent::KeyItem keyItem;
        keyItem.SetKeyCode(keycode);
        keyItem.SetPressed(isPressed);
        keyItem.SetDownTime(keycode);
        keyItem.SetDeviceId(DEVICE_ID);
        return keyItem;
    }

    void CheckTouchMoveEntity(const PointerEvent::PointerItem &pointerItem)
    {
        ASSERT_EQ(handler_->touchMoveEntity_.xValue, pointerItem.GetWindowX());
        ASSERT_EQ(handler_->touchMoveEntity_.yValue, pointerItem.GetWindowY());
        ASSERT_EQ(handler_->touchMoveEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
        std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_WALK);
        ASSERT_EQ(handler_->touchMoveEntity_.pointerId, pair.second);
    }

public:
    std::shared_ptr<DpadKeyToTouchHandlerEx> handler_;
    std::shared_ptr<InputToTouchContext> context_;
    std::shared_ptr<MMI::KeyEvent> keyEvent_;
    KeyToTouchMappingInfo mappingInfo_;
    DeviceInfo deviceInfo_;
};

/**
 * @tc.name: HandleKeyDown_001
 * @tc.desc: when key is down and context_->isWalking is false, can send touch down command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_001, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    keyEvent_->SetKeyCode(KEY_UP_CODE);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_UP_CODE, true));
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);
    std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_WALK);
    ASSERT_TRUE(pair.first);
    int32_t pointerId = pair.second;
    ASSERT_TRUE(context_->isWalking);
    ASSERT_EQ(context_->currentWalking.mappingType, mappingInfo_.mappingType);
    ASSERT_EQ(handler_->touchDownEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchDownEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->touchDownEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(handler_->touchDownEntity_.pointerId, pointerId);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE - RADIUS);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyDown_002
 * @tc.desc: when key is down and context_->isWalking is true
 * and context->currentWalking.mappingType is MOUSE_RIGHT_KEY_WALKING_TO_TOUCH, will discard the event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_002, TestSize.Level1)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    context_->currentWalking.mappingType = MOUSE_RIGHT_KEY_WALKING_TO_TOUCH;
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(context_->currentWalking, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) == context_->pointerItems.end());
    ASSERT_FALSE(handler_->hasTouchEvent_);
}

/**
 * @tc.name: HandleKeyDown_003
 * @tc.desc: when key is down and keycode is up and context_->isWalking is true
 * and have the pressed keycodes of up, it will move to the up
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_003, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_UP_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_UP_CODE, true));
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(mappingInfo_, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE - RADIUS);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyDown_004
 * @tc.desc: when key is down and keycode is down and context_->isWalking is true
 * and have the pressed keycodes of down, it will move to the down
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_004, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_DOWN_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_DOWN_CODE, true));
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(mappingInfo_, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE + RADIUS);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyDown_005
 * @tc.desc: when key is down and keycode is left and context_->isWalking is true
 * and have the pressed keycodes of left, it will move to the left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_005, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_LEFT_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_LEFT_CODE, true));
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(mappingInfo_, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE - RADIUS);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyDown_006
 * @tc.desc: when key is down and keycode is right and context_->isWalking is true
 * and have the pressed keycodes of right, it will move to the right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_006, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_RIGHT_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_RIGHT_CODE, true));
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(mappingInfo_, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE + RADIUS);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyDown_007
 * @tc.desc: when key is down and keycode is left and context_->isWalking is true
 * and have the pressed keycodes of up and left and down,
 * and the downTime of down is more then the downTime of up
 * then it will move to the up and left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_007, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_LEFT_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_DOWN_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_LEFT_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_RIGHT_CODE, false));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_UP_CODE, true));
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(mappingInfo_, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), 376);
    ASSERT_EQ(pointerItem.GetWindowY(), 838);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyDown_008
 * @tc.desc: when key is down and keycode is left and context_->isWalking is true
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of down is less then the downTime of up
 * then it will move to the down and left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_008, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_LEFT_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    KeyEvent::KeyItem keyItem = BuildKeyItem(KEY_DOWN_CODE, true);
    keyItem.SetDownTime(1000);
    keyEvent_->AddKeyItem(keyItem);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_LEFT_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_RIGHT_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_UP_CODE, true));
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(mappingInfo_, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), 376);
    ASSERT_EQ(pointerItem.GetWindowY(), 1163);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyDown_009
 * @tc.desc: when key is down and keycode is right and context_->isWalking is true
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of down is more then the downTime of up
 * then it will move to the up and right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_009, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_RIGHT_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    KeyEvent::KeyItem keyItem = BuildKeyItem(KEY_DOWN_CODE, true);
    keyEvent_->AddKeyItem(keyItem);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_LEFT_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_RIGHT_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_UP_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE, true));
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(mappingInfo_, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), 701);
    ASSERT_EQ(pointerItem.GetWindowY(), 838);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyDown_010
 * @tc.desc: when key is down and keycode is right and context_->isWalking is true
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of down is less then the downTime of up
 * then it will move to the down and right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_010, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_RIGHT_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    KeyEvent::KeyItem keyItem = BuildKeyItem(KEY_DOWN_CODE, true);
    keyItem.SetDownTime(1000);
    keyEvent_->AddKeyItem(keyItem);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_LEFT_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_RIGHT_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_UP_CODE, true));
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(mappingInfo_, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), 701);
    ASSERT_EQ(pointerItem.GetWindowY(), 1163);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyDown_011
 * @tc.desc: when key is down and keycode is up and context_->isWalking is true
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of left is less then the downTime of right
 * then it will move to the up and left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_011, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_UP_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_DOWN_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_LEFT_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_RIGHT_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_UP_CODE, true));
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(mappingInfo_, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), 376);
    ASSERT_EQ(pointerItem.GetWindowY(), 838);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyDown_012
 * @tc.desc: when key is down and keycode is up and context_->isWalking is true
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of left is more then the downTime of right
 * then it will move to the up and right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_012, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_UP_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_DOWN_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_LEFT_CODE, true));
    KeyEvent::KeyItem keyItem = BuildKeyItem(KEY_RIGHT_CODE, true);
    keyItem.SetDownTime(1000);
    keyEvent_->AddKeyItem(keyItem);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_UP_CODE, true));
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(mappingInfo_, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), 701);
    ASSERT_EQ(pointerItem.GetWindowY(), 838);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyDown_013
 * @tc.desc: when key is down and keycode is down and context_->isWalking is true
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of left is less then the downTime of right
 * then it will move to the down and left
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_013, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_DOWN_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_DOWN_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_LEFT_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_RIGHT_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_UP_CODE, true));
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(mappingInfo_, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), 376);
    ASSERT_EQ(pointerItem.GetWindowY(), 1163);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyDown_014
 * @tc.desc: when key is down and keycode is down and context_->isWalking is true
 * and have the pressed keycodes of up and down and left and right,
 * and the downTime of left is more then the downTime of right
 * then it will move to the down and right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyDown_014, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_DOWN_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_DOWN_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_LEFT_CODE, true));
    KeyEvent::KeyItem keyItem = BuildKeyItem(KEY_RIGHT_CODE, true);
    keyItem.SetDownTime(1000);
    keyEvent_->AddKeyItem(keyItem);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_UP_CODE, true));
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentWalking(mappingInfo_, pointerId);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), 701);
    ASSERT_EQ(pointerItem.GetWindowY(), 1163);
    CheckTouchMoveEntity(pointerItem);
}

/**
 * @tc.name: HandleKeyUp_001
 * @tc.desc: when key is up and context_->isWalking is true, and need exit walking,
 * can send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyUp_001, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);
    std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_WALK);
    ASSERT_TRUE(pair.first);
    int32_t pointerId = pair.second;
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_UP_CODE, false));
    KeyEvent::KeyItem keyItem = BuildKeyItem(KEY_DOWN_CODE, true);
    keyItem.SetDeviceId(OTHER_DEVICE_ID);
    keyEvent_->AddKeyItem(keyItem);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_CODE, true));
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_FALSE(context_->isWalking);
    ASSERT_EQ(context_->currentWalking.xValue, 0);
    ASSERT_EQ(context_->currentWalking.yValue, 0);
    ASSERT_TRUE(context_->pointerItems.find(pointerId) == context_->pointerItems.end());
    ASSERT_EQ(handler_->touchUpEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchUpEntity_.pointerId, pointerId);
}

/**
 * @tc.name: HandleKeyUp_002
 * @tc.desc: when key is up and context_->isWalking is false, cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyUp_002, TestSize.Level1)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);
    std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_WALK);
    ASSERT_TRUE(pair.first);
    int32_t pointerId = pair.second;
    context_->isWalking = false;
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
}

/**
 * @tc.name: HandleKeyUp_003
 * @tc.desc: when key is up and context_->isWalking is true
 * and context->currentWalking.mappingType is not DPAD_KEY_TO_TOUCH,
 * cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyUp_003, TestSize.Level1)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);
    std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_WALK);
    int32_t pointerId = pair.second;
    context_->currentWalking.mappingType = MOUSE_LEFT_FIRE_TO_TOUCH;
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_TRUE(context_->isWalking);
    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
}

/**
 * @tc.name: HandleKeyUp_004
 * @tc.desc: when key is up and keycode is left and context_->isWalking is true and other dpad's key in keyItems,
 * and have the pressed keycodes of up and down and right,
 * and the downTime of the right is largest and the the downTime of up is less than the downTime of down
 * then it will move to the up and right
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(DpadKeyToTouchHandlerTest, HandleKeyUp_004, TestSize.Level0)
{
    keyEvent_->SetKeyCode(KEY_LEFT_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);
    std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_WALK);
    int32_t pointerId = pair.second;

    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_DOWN_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_UP_CODE, true));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_LEFT_CODE, false));
    keyEvent_->AddKeyItem(BuildKeyItem(KEY_RIGHT_CODE, true));
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), 701);
    ASSERT_EQ(pointerItem.GetWindowY(), 838);
    CheckTouchMoveEntity(pointerItem);
}
}
}