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
#include "skill_key_to_touch_handler.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
namespace {
const int32_t X_VALUE = 2045;
const int32_t Y_VALUE = 1003;
const int32_t SKILL_RANGE = 450;
const int32_t RADIUS = 190;
const int32_t KEY_CODE = 2301;
const int32_t X_CENTER_VALUE = 1360;
const int32_t Y_CENTER_VALUE = 630;
}
class SkillKeyToTouchHandlerEx : public SkillKeyToTouchHandler {
public:
    void BuildAndSendPointerEvent(std::shared_ptr<InputToTouchContext> &context_,
                                  const TouchEntity &touchEntity) override;

public:
    TouchEntity touchEntity_;
};

void SkillKeyToTouchHandlerEx::BuildAndSendPointerEvent(std::shared_ptr<InputToTouchContext> &context_,
                                                        const TouchEntity &touchEntity)
{
    BaseKeyToTouchHandler::BuildAndSendPointerEvent(context_, touchEntity);
    touchEntity_ = touchEntity;
}

class SkillKeyToTouchHandlerTest : public testing::Test {
public:
    void SetUp() override
    {
        handler_ = std::make_shared<SkillKeyToTouchHandlerEx>();
        context_ = std::make_shared<InputToTouchContext>();
        keyEvent_ = KeyEvent::Create();
        keyEvent_->SetKeyCode(KEY_CODE);
        pointerEvent_ = PointerEvent::Create();
        pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
        mappingInfo_ = BuildKeyToTouchMappingInfo();
    }

    static KeyToTouchMappingInfo BuildKeyToTouchMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.keyCode = KEY_CODE;
        info.mappingType = MappingTypeEnum::SKILL_KEY_TO_TOUCH;
        info.xValue = X_VALUE;
        info.yValue = Y_VALUE;
        info.skillRange = SKILL_RANGE;
        info.radius = RADIUS;
        return info;
    }

    int32_t SendDownEvent()
    {
        keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
        handler_->HandleKeyEvent(context_, keyEvent_, deviceInfo_, mappingInfo_);
        std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_SKILL);
        int32_t pointerId = pair.second;
        return pointerId;
    }

    void TearDown() override
    {
        context_->ResetCurrentSkillKeyInfo();
    }

public:
    std::shared_ptr<SkillKeyToTouchHandlerEx> handler_;
    std::shared_ptr<InputToTouchContext> context_;
    std::shared_ptr<MMI::KeyEvent> keyEvent_;
    std::shared_ptr<MMI::PointerEvent> pointerEvent_;
    KeyToTouchMappingInfo mappingInfo_;
    DeviceInfo deviceInfo_;
};

/**
 * @tc.name: HandleKeyEvent_001
 * @tc.desc: check parameter is invalid
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SkillKeyToTouchHandlerTest, HandleKeyEvent_001, TestSize.Level1)
{
    handler_->HandleKeyEvent(context_, nullptr, deviceInfo_, mappingInfo_);
    ASSERT_FALSE(context_->isSkillOperating);
    ASSERT_EQ(handler_->touchEntity_.xValue, 0);
    ASSERT_EQ(handler_->touchEntity_.yValue, 0);
}

/**
 * @tc.name: HandleKeyEvent_002
 * @tc.desc: when key is down and context_->isSkillOperating is false, can send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SkillKeyToTouchHandlerTest, HandleKeyEvent_002, TestSize.Level0)
{
    int32_t pointerId = SendDownEvent();

    ASSERT_TRUE(context_->isSkillOperating);
    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    PointerEvent::PointerItem pointerItem = context_->pointerItems[pointerId];
    ASSERT_EQ(pointerItem.GetWindowX(), X_VALUE);
    ASSERT_EQ(pointerItem.GetWindowY(), Y_VALUE);
    ASSERT_EQ(context_->currentSkillKeyInfo.mappingType, mappingInfo_.mappingType);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(handler_->touchEntity_.pointerId, pointerId);
}

/**
 * @tc.name: HandleKeyEvent_003
 * @tc.desc: when key is down and context_->isSkillOperating is true, cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SkillKeyToTouchHandlerTest, HandleKeyEvent_003, TestSize.Level1)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentSkillKeyInfo(mappingInfo_, pointerId);
    handler_->HandleKeyEvent(context_, keyEvent_, deviceInfo_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) == context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.xValue, 0);
    ASSERT_EQ(handler_->touchEntity_.yValue, 0);
}

/**
 * @tc.name: HandleKeyEvent_004
 * @tc.desc: when key is up and context_->isSkillOperating is true, can send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SkillKeyToTouchHandlerTest, HandleKeyEvent_004, TestSize.Level0)
{
    int32_t pointerId = SendDownEvent();

    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyEvent(context_, keyEvent_, deviceInfo_, mappingInfo_);

    ASSERT_FALSE(context_->isSkillOperating);
    ASSERT_EQ(context_->currentSkillKeyInfo.mappingType, 0);
    ASSERT_TRUE(context_->pointerItems.find(pointerId) == context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchEntity_.pointerId, pointerId);
}

/**
 * @tc.name: HandleKeyEvent_005
 * @tc.desc: when key is up and context_->isSkillOperating is false, cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SkillKeyToTouchHandlerTest, HandleKeyEvent_005, TestSize.Level0)
{
    int32_t pointerId = SendDownEvent();

    context_->isSkillOperating = false;
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyEvent(context_, keyEvent_, deviceInfo_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
}

/**
 * @tc.name: HandleKeyEvent_006
 * @tc.desc: when key is up and context_->isSkillOperating is true and keycode is not same with template,
 * cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SkillKeyToTouchHandlerTest, HandleKeyEvent_006, TestSize.Level0)
{
    int32_t pointerId = SendDownEvent();

    keyEvent_->SetKeyCode(0);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyEvent(context_, keyEvent_, deviceInfo_, mappingInfo_);

    ASSERT_TRUE(context_->isSkillOperating);
    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
}

/**
 * @tc.name: HandleKeyEvent_007
 * @tc.desc: when key is up and context_->pointItems cannot find point
 * cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SkillKeyToTouchHandlerTest, HandleKeyEvent_007, TestSize.Level0)
{
    int32_t pointerId = SendDownEvent();

    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    context_->pointerItems.clear();
    handler_->HandleKeyEvent(context_, keyEvent_, deviceInfo_, mappingInfo_);

    ASSERT_TRUE(context_->isSkillOperating);
}

/**
 * @tc.name: HandlePointerEvent_001
 * @tc.desc: when it's not move event, cannot send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SkillKeyToTouchHandlerTest, HandlePointerEvent_001, TestSize.Level0)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.empty());
    ASSERT_EQ(handler_->touchEntity_.xValue, 0);
    ASSERT_EQ(handler_->touchEntity_.yValue, 0);
}

/**
 * @tc.name: HandlePointerEvent_002
 * @tc.desc: when context->isSkillOperating is false, cannot send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SkillKeyToTouchHandlerTest, HandlePointerEvent_002, TestSize.Level0)
{
    context_->isSkillOperating = false;
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.empty());
    ASSERT_EQ(handler_->touchEntity_.xValue, 0);
    ASSERT_EQ(handler_->touchEntity_.yValue, 0);
}

/**
 * @tc.name: HandlePointerEvent_003
 * @tc.desc: when pointerEvent is nullptr, cannot send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SkillKeyToTouchHandlerTest, HandlePointerEvent_003, TestSize.Level0)
{
    handler_->HandlePointerEvent(context_, nullptr, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.empty());
    ASSERT_EQ(handler_->touchEntity_.xValue, 0);
    ASSERT_EQ(handler_->touchEntity_.yValue, 0);
}

/**
 * @tc.name: HandlePointerEvent_005
 * @tc.desc: when The distance from the mouse to the center point exceeds the skill range,
 * the skill key movement radius is set to distance * radius / skillRange * skillRangeScale.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SkillKeyToTouchHandlerTest, HandlePointerEvent_005, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentSkillKeyInfo(mappingInfo_, pointerId);
    context_->windowInfoEntity.xCenter = X_CENTER_VALUE;
    context_->windowInfoEntity.yCenter = Y_CENTER_VALUE;

    PointerEvent::PointerItem pointerItem;
    pointerItem.SetWindowX(1540);
    pointerItem.SetWindowY(100);
    pointerEvent_->AddPointerItem(pointerItem);

    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.xValue, 2120);
    ASSERT_EQ(handler_->touchEntity_.yValue, 753);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchEntity_.pointerId, pointerId);
}

/**
 * @tc.name: HandlePointerEvent_006
 * @tc.desc: when The distance from the mouse to the center point less than the skill range,
 * the skill key movement radius is set to distance * radius / (skillRange * skillRangeScale).
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SkillKeyToTouchHandlerTest, HandlePointerEvent_006, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentSkillKeyInfo(mappingInfo_, pointerId);
    context_->windowInfoEntity.xCenter = X_CENTER_VALUE;
    context_->windowInfoEntity.yCenter = Y_CENTER_VALUE;
    PointerEvent::PointerItem pointerItem;
    pointerItem.SetWindowX(1540);
    pointerItem.SetWindowY(230);
    pointerEvent_->AddPointerItem(pointerItem);

    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.xValue, 2120);
    ASSERT_EQ(handler_->touchEntity_.yValue, 808);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchEntity_.pointerId, pointerId);
}
}
}