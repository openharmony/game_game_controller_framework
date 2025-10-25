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
#include "single_key_to_touch_handler.h"
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
}
class SingleKeyToTouchHandlerEx : public SingleKeyToTouchHandler {
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
        SingleKeyToTouchHandler::HandleKeyDown(context, keyEvent, mappingInfo, deviceInfo);
    }

    void HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                     const std::shared_ptr<MMI::KeyEvent> &keyEvent, const DeviceInfo &deviceInfo) override
    {
        SingleKeyToTouchHandler::HandleKeyUp(context, keyEvent, deviceInfo);
    }

public:
    TouchEntity touchEntity_;
};

class SingleKeyToTouchHandlerTest : public testing::Test {
public:
    void SetUp() override
    {
        handler_ = std::make_shared<SingleKeyToTouchHandlerEx>();
        context_ = std::make_shared<InputToTouchContext>();
        keyEventA_ = KeyEvent::Create();
        keyEventA_->SetKeyCode(KEY_CODE_A);
        keyEventB_ = KeyEvent::Create();
        keyEventB_->SetKeyCode(KEY_CODE_B);
        mappingInfoA_ = BuildKeyToTouchMappingInfo(KEY_CODE_A);
        mappingInfoB_ = BuildKeyToTouchMappingInfo(KEY_CODE_B);
    }

    static KeyToTouchMappingInfo BuildKeyToTouchMappingInfo(int32_t keyCode)
    {
        KeyToTouchMappingInfo info;
        info.keyCode = keyCode;
        info.mappingType = MappingTypeEnum::SINGE_KEY_TO_TOUCH;
        info.xValue = X_VALUE;
        info.yValue = Y_VALUE;
        return info;
    }

    int32_t SendDownEventA()
    {
        keyEventA_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
        handler_->HandleKeyDown(context_, keyEventA_, mappingInfoA_, deviceInfo_);
        std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_A);
        int32_t pointerId = pair.second;
        return pointerId;
    }

    int32_t SendDownEventB()
    {
        keyEventB_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
        handler_->HandleKeyDown(context_, keyEventB_, mappingInfoB_, deviceInfo_);
        std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_B);
        int32_t pointerId = pair.second;
        return pointerId;
    }

    void TearDown() override
    {
        context_->ResetCurrentObserving();
    }

public:
    std::shared_ptr<SingleKeyToTouchHandlerEx> handler_;
    std::shared_ptr<InputToTouchContext> context_;
    std::shared_ptr<MMI::KeyEvent> keyEventA_;
    std::shared_ptr<MMI::KeyEvent> keyEventB_;
    KeyToTouchMappingInfo mappingInfoA_;
    KeyToTouchMappingInfo mappingInfoB_;
    DeviceInfo deviceInfo_;
};

/**
 * @tc.name: HandleKeyDown_001
 * @tc.desc: when key is down and the keycode is not pressed down, can send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SingleKeyToTouchHandlerTest, HandleKeyDown_001, TestSize.Level0)
{
    int32_t pointerId = SendDownEventA();

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE);
    ASSERT_EQ(context_->currentSingleKeyMap[KEY_CODE_A].mappingType, mappingInfoA_.mappingType);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(handler_->touchEntity_.pointerId, pointerId);
}

/**
 * @tc.name: HandleKeyDown_002
 * @tc.desc: when key is down and the keycode has been pressed down, cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SingleKeyToTouchHandlerTest, HandleKeyDown_002, TestSize.Level1)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentSingleKeyInfo(mappingInfoA_, pointerId);
    handler_->HandleKeyDown(context_, keyEventA_, mappingInfoA_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) == context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.xValue, 0);
    ASSERT_EQ(handler_->touchEntity_.yValue, 0);
}

/**
 * @tc.name: HandleKeyDown_003
 * @tc.desc:Press two different buttons one after another, and then lift them up one after another
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SingleKeyToTouchHandlerTest, HandleKeyDown_003, TestSize.Level0)
{
    int32_t pointerIdA = SendDownEventA();
    ASSERT_TRUE(context_->pointerItems.find(pointerIdA) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerIdA];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE);
    ASSERT_EQ(context_->currentSingleKeyMap[KEY_CODE_A].mappingType, mappingInfoA_.mappingType);
    ASSERT_EQ(handler_->touchEntity_.pointerId, pointerIdA);

    int32_t pointerIdB = SendDownEventB();
    pointerItem = context_->pointerItems[pointerIdB];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE);
    ASSERT_EQ(context_->currentSingleKeyMap[KEY_CODE_B].mappingType, mappingInfoB_.mappingType);
    ASSERT_EQ(handler_->touchEntity_.pointerId, pointerIdB);

    keyEventA_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEventA_, deviceInfo_);
    ASSERT_FALSE(context_->HasSingleKeyDown(KEY_CODE_A));
    ASSERT_TRUE(context_->pointerItems.find(pointerIdA) == context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchEntity_.pointerId, pointerIdA);

    keyEventB_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEventB_, deviceInfo_);
    ASSERT_FALSE(context_->HasSingleKeyDown(KEY_CODE_B));
    ASSERT_TRUE(context_->pointerItems.find(pointerIdB) == context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchEntity_.pointerId, pointerIdB);
}



/**
 * @tc.name: HandleKeyUp_001
 * @tc.desc: when key is up and context_->HasSingleKeyDown is true, can send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SingleKeyToTouchHandlerTest, HandleKeyUp_001, TestSize.Level0)
{
    int32_t pointerId = SendDownEventA();
    ASSERT_TRUE(context_->HasSingleKeyDown(mappingInfoA_.keyCode));
    keyEventA_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEventA_, deviceInfo_);

    ASSERT_FALSE(context_->HasSingleKeyDown(mappingInfoA_.keyCode));
    ASSERT_TRUE(context_->pointerItems.find(pointerId) == context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchEntity_.pointerId, pointerId);
}

/**
 * @tc.name: HandleKeyUp_002
 * @tc.desc: when key is up and context_->HasSingleKeyDown is false, cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SingleKeyToTouchHandlerTest, HandleKeyUp_002, TestSize.Level0)
{
    int32_t pointerId = SendDownEventA();

    keyEventA_->SetKeyCode(KEY_CODE_B);
    keyEventA_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEventA_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
}
}
}