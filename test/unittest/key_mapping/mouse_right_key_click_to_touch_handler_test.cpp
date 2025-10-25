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
#include "mouse_right_key_click_to_touch_handler.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
namespace {
const int32_t X_VALUE = 2045;
const int32_t Y_VALUE = 863;
const int32_t MOUSE_X_VALUE = 2235;
const int32_t MOUSE_Y_VALUE = 650;
const int32_t MAX_WIDTH = 2720;
const int32_t MAX_HEIGHT = 1260;
const int32_t X_STEP = 1;
const int32_t Y_STEP = 2;
const int32_t MOUSE_MOVE_DISTANCE = 10;
}
class MouseRightKeyClickToTouchHandlerEx : public MouseRightKeyClickToTouchHandler {
public:
    void BuildAndSendPointerEvent(std::shared_ptr<InputToTouchContext> &context_,
                                  const TouchEntity &touchEntity) override
    {
        BaseKeyToTouchHandler::BuildAndSendPointerEvent(context_, touchEntity);
        touchEntity_ = touchEntity;
    }

public:
    TouchEntity touchEntity_;
};

class MouseRightKeyClickToTouchHandlerTest : public testing::Test {
public:
    void SetUp() override
    {
        handler_ = std::make_shared<MouseRightKeyClickToTouchHandlerEx>();
        context_ = std::make_shared<InputToTouchContext>();
        pointerEvent_ = PointerEvent::Create();
        pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
        pointerEvent_->SetButtonId(1);
        pointerItem_.SetWindowX(MOUSE_X_VALUE);
        pointerItem_.SetWindowY(MOUSE_Y_VALUE);
        pointerEvent_->AddPointerItem(pointerItem_);
        mappingInfo_ = BuildKeyToTouchMappingInfo();
        context_->windowInfoEntity.maxWidth = MAX_WIDTH;
        context_->windowInfoEntity.maxHeight = MAX_HEIGHT;
    }

    static KeyToTouchMappingInfo BuildKeyToTouchMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.mappingType = MappingTypeEnum::MOUSE_RIGHT_KEY_CLICK_TO_TOUCH;
        info.xValue = X_VALUE;
        info.yValue = Y_VALUE;
        info.xStep = X_STEP;
        info.yStep = Y_STEP;
        return info;
    }

    void TearDown() override
    {
        context_->ResetCurrentMouseRightClick();
    }

    int32_t SendMouseRightDownEvent()
    {
        context_->isMouseRightClickOperating = false;
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
        handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);
        std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_MOUSE_RIGHT);
        int32_t pointerId = pair.second;
        return pointerId;
    }

public:
    std::shared_ptr<MouseRightKeyClickToTouchHandlerEx> handler_;
    std::shared_ptr<InputToTouchContext> context_;
    std::shared_ptr<MMI::PointerEvent> pointerEvent_;
    KeyToTouchMappingInfo mappingInfo_;
    PointerEvent::PointerItem pointerItem_;
};

/**
 * @tc.name: HandlePointerEvent_001
 * @tc.desc: when it's mouse right-button down event and isMouseRightClickOperating is false,
 * send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyClickToTouchHandlerTest, HandlePointerEvent_001, TestSize.Level0)
{
    int32_t pointerId = SendMouseRightDownEvent();
    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    ASSERT_TRUE(context_->isMouseRightClickOperating);
    ASSERT_EQ(handler_->touchEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
}

/**
 * @tc.name: HandlePointerEvent_002
 * @tc.desc: when it's mouse right-button down event and isMouseRightClickOperating is true,
 * discard the event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyClickToTouchHandlerTest, HandlePointerEvent_002, TestSize.Level1)
{
    context_->isMouseRightClickOperating = true;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);

    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_MOUSE_RIGHT);
    ASSERT_FALSE(pair.first);
}

/**
 * @tc.name: HandlePointerEvent_004
 * @tc.desc: when it's mouse move event, discard the event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyClickToTouchHandlerTest, HandlePointerEvent_004, TestSize.Level1)
{
    context_->isMouseRightClickOperating = false;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->SetButtonId(0);

    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_MOUSE_RIGHT);
    ASSERT_FALSE(pair.first);
}

/**
 * @tc.name: HandlePointerEvent_005
 * @tc.desc: when it's mouse left-btn event, discard the event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyClickToTouchHandlerTest, HandlePointerEvent_005, TestSize.Level1)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent_->SetButtonId(0);

    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_MOUSE_RIGHT);
    ASSERT_FALSE(pair.first);
    ASSERT_FALSE(context_->isMouseRightClickOperating);
}

/**
 * @tc.name: HandlePointerEvent_006
 * @tc.desc: when it's mouse right-button up event and isMouseRightClickOperating is true,
 *  send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyClickToTouchHandlerTest, HandlePointerEvent_006, TestSize.Level0)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(SINGLE_POINT_ID) == context_->pointerItems.end());
    ASSERT_FALSE(context_->isMouseRightClickOperating);
    ASSERT_EQ(handler_->touchEntity_.pointerId, SINGLE_POINT_ID);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
}

/**
 * @tc.name: HandlePointerEvent_007
 * @tc.desc: when it's mouse right-button up event and isMouseRightClickOperating is false,
 *  discard the event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyClickToTouchHandlerTest, HandlePointerEvent_007, TestSize.Level0)
{
    int32_t pointerId = SendMouseRightDownEvent();

    context_->isMouseRightClickOperating = false;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
}

/**
 * @tc.name: HandlePointerEvent_008
 * @tc.desc: when it's mouse right-button down event and isMouseLeftFireOperating is true,
 * discard the event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyClickToTouchHandlerTest, HandlePointerEvent_008, TestSize.Level1)
{
    context_->isMouseLeftFireOperating = true;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);

    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_MOUSE_RIGHT);
    ASSERT_FALSE(pair.first);
    ASSERT_FALSE(context_->isMouseRightClickOperating);
}
}
}