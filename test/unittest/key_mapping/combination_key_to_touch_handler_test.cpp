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
#include "refbase.h"
#include "combination_key_to_touch_handler.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
namespace {
const int32_t X_VALUE = 2045;
const int32_t Y_VALUE = 1003;
const int32_t FIRST_KEY_CODE = 2025;
const int32_t KEY_CODE = 2301;
}
class CombinationKeyToTouchHandlerEx : public CombinationKeyToTouchHandler {
public:
    void BuildAndSendPointerEvent(std::shared_ptr<InputToTouchContext> &context_,
                                  const TouchEntity &touchEntity) override
    {
        BaseKeyToTouchHandler::BuildAndSendPointerEvent(context_, touchEntity);
        touchEntity_ = touchEntity;
    }

    void HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                       const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                       const KeyToTouchMappingInfo &mappingInfo,
                       const DeviceInfo &deviceInfo) override
    {
        CombinationKeyToTouchHandler::HandleKeyDown(context, keyEvent, mappingInfo, deviceInfo);
    }

    void HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                     const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                     const DeviceInfo &deviceInfo) override
    {
        CombinationKeyToTouchHandler::HandleKeyUp(context, keyEvent, deviceInfo);
    }

public:
    TouchEntity touchEntity_;
};

class CombinationKeyToTouchHandlerTest : public testing::Test {
public:
    void SetUp()
    {
        handler_ = std::make_shared<CombinationKeyToTouchHandlerEx>();
        context_ = std::make_shared<InputToTouchContext>();
        keyEvent_ = KeyEvent::Create();
        keyEvent_->SetKeyCode(KEY_CODE);
        mappingInfo_ = BuildKeyToTouchMappingInfo();
    }

    KeyToTouchMappingInfo BuildKeyToTouchMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.mappingType = MappingTypeEnum::COMBINATION_KEY_TO_TOUCH;
        info.xValue = X_VALUE;
        info.yValue = Y_VALUE;
        info.combinationKeys.push_back(FIRST_KEY_CODE);
        info.combinationKeys.push_back(KEY_CODE);
        return info;
    }

public:
    std::shared_ptr<CombinationKeyToTouchHandlerEx> handler_;
    std::shared_ptr<InputToTouchContext> context_;
    std::shared_ptr<MMI::KeyEvent> keyEvent_;
    KeyToTouchMappingInfo mappingInfo_;
    DeviceInfo deviceInfo_;
};

/**
 * @tc.name: HandleKeyDown_001
 * @tc.desc: when key is down and context_->isCombinationKeyOperating is false, can send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CombinationKeyToTouchHandlerTest, HandleKeyDown_001, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->isCombinationKeyOperating);
    ASSERT_TRUE(context_->pointerItems.find(COMBINATION_POINT_ID) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[COMBINATION_POINT_ID];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE);
    ASSERT_EQ(context_->currentCombinationKey.mappingType, mappingInfo_.mappingType);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(handler_->touchEntity_.pointerId, COMBINATION_POINT_ID);
}

/**
 * @tc.name: HandleKeyDown_002
 * @tc.desc: when key is down and mappingInfo_.combinationKeys's size is not equal to 2,
 * cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CombinationKeyToTouchHandlerTest, HandleKeyDown_002, TestSize.Level1)
{

    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    mappingInfo_.combinationKeys.push_back(1);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(COMBINATION_POINT_ID) == context_->pointerItems.end());
}

/**
 * @tc.name: HandleKeyDown_003
 * @tc.desc: when key is down and context_->isCombinationKeyOperating is true,
 * cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CombinationKeyToTouchHandlerTest, HandleKeyDown_003, TestSize.Level1)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    context_->isCombinationKeyOperating = true;
    context_->currentCombinationKey.combinationKeys.clear();
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(COMBINATION_POINT_ID) == context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.xValue, 0);
    ASSERT_EQ(handler_->touchEntity_.yValue, 0);
}

/**
 * @tc.name: HandleKeyUp_001
 * @tc.desc: when key is up and context_->isCombinationKeyOperating is true, can send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CombinationKeyToTouchHandlerTest, HandleKeyUp_001, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_FALSE(context_->isCombinationKeyOperating);
    ASSERT_EQ(context_->currentSingleKey.xValue, 0);
    ASSERT_EQ(context_->currentSingleKey.yValue, 0);
    ASSERT_TRUE(context_->pointerItems.find(COMBINATION_POINT_ID) == context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchEntity_.pointerId, COMBINATION_POINT_ID);
}

/**
 * @tc.name: HandleKeyUp_002
 * @tc.desc: when key is up and context_->isCombinationKeyOperating is false, cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CombinationKeyToTouchHandlerTest, HandleKeyUp_002, TestSize.Level1)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    context_->isCombinationKeyOperating = false;
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(COMBINATION_POINT_ID) != context_->pointerItems.end());
}

/**
 * @tc.name: HandleKeyUp_003
 * @tc.desc: when key is up and context_->isCombinationKeyOperating is true
 * and keycode is not same with combinationKeys[1],
 * cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CombinationKeyToTouchHandlerTest, HandleKeyUp_003, TestSize.Level1)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(FIRST_KEY_CODE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_TRUE(context_->isCombinationKeyOperating);
    ASSERT_TRUE(context_->pointerItems.find(COMBINATION_POINT_ID) != context_->pointerItems.end());
}

/**
 * @tc.name: HandleKeyUp_004
 * @tc.desc: when key is up and context_->isCombinationKeyOperating is true
 * and context->currentCombinationKey.combinationKeys is not equal to 2,
 * cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CombinationKeyToTouchHandlerTest, HandleKeyUp_004, TestSize.Level1)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    context_->currentCombinationKey.combinationKeys.push_back(1);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_TRUE(context_->isCombinationKeyOperating);
    ASSERT_TRUE(context_->pointerItems.find(COMBINATION_POINT_ID) != context_->pointerItems.end());
}
}
}