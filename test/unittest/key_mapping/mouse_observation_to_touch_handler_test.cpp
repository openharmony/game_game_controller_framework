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
#include "mouse_observation_to_touch_handler.h"

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
const int32_t MIN_EDGE = 1;
}
class MouseObservationToTouchHandlerEx : public MouseObservationToTouchHandler {
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

class MouseObservationToTouchHandlerTest : public testing::Test {
public:
    void SetUp()
    {
        handler_ = std::make_shared<MouseObservationToTouchHandlerEx>();
        context_ = std::make_shared<InputToTouchContext>();
        pointerEvent_ = PointerEvent::Create();
        pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
        pointerItem_.SetWindowX(MOUSE_X_VALUE);
        pointerItem_.SetWindowY(MOUSE_Y_VALUE);
        pointerEvent_->AddPointerItem(pointerItem_);
        mappingInfo_ = BuildKeyToTouchMappingInfo();
        context_->windowInfoEntity.maxWidth = MAX_WIDTH;
        context_->windowInfoEntity.maxHeight = MAX_HEIGHT;
    }

    KeyToTouchMappingInfo BuildKeyToTouchMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.mappingType = MappingTypeEnum::MOUSE_OBSERVATION_TO_TOUCH;
        info.xValue = X_VALUE;
        info.yValue = Y_VALUE;
        info.xStep = X_STEP;
        info.yStep = Y_STEP;
        return info;
    }

    PointerEvent::PointerItem BuildPointerItem(int32_t xVal, int32_t yVal)
    {
        PointerEvent::PointerItem pointerItem;
        pointerItem.SetWindowX(xVal);
        pointerItem.SetWindowY(yVal);
        return pointerItem;
    }

public:
    std::shared_ptr<MouseObservationToTouchHandlerEx> handler_;
    std::shared_ptr<InputToTouchContext> context_;
    std::shared_ptr<MMI::PointerEvent> pointerEvent_;
    KeyToTouchMappingInfo mappingInfo_;
    PointerEvent::PointerItem pointerItem_;
};

/**
 * @tc.name: HandlePointerEvent_001
 * @tc.desc: when it's mouse right-button down event, send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseObservationToTouchHandlerTest, HandlePointerEvent_001, TestSize.Level0)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent_->SetButtonId(1);

    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) != context_->pointerItems.end());
    ASSERT_TRUE(context_->isPerspectiveObserving);
    ASSERT_EQ(context_->currentPerspectiveObserving.mappingType, MOUSE_OBSERVATION_TO_TOUCH);
    ASSERT_EQ(context_->lastMousePointer.GetWindowX(), MOUSE_X_VALUE);
    ASSERT_EQ(context_->lastMousePointer.GetWindowY(), MOUSE_Y_VALUE);
    ASSERT_EQ(handler_->touchEntity_.pointerId, OBSERVATION_POINT_ID);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
}

/**
 * @tc.name: HandlePointerEvent_002
 * @tc.desc: when it's mouse right-button up event, send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseObservationToTouchHandlerTest, HandlePointerEvent_002, TestSize.Level0)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent_->SetButtonId(1);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) == context_->pointerItems.end());
    ASSERT_FALSE(context_->isPerspectiveObserving);
    ASSERT_EQ(context_->currentPerspectiveObserving.mappingType, 0);
    ASSERT_EQ(context_->lastMousePointer.GetWindowX(), 0);
    ASSERT_EQ(context_->lastMousePointer.GetWindowY(), 0);
    ASSERT_EQ(handler_->touchEntity_.pointerId, OBSERVATION_POINT_ID);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
}

/**
 * @tc.name: HandlePointerEvent_003
 * @tc.desc: when it's mouse right-button down event and isPerspectiveObserving is true, no send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseObservationToTouchHandlerTest, HandlePointerEvent_003, TestSize.Level1)
{
    context_->isPerspectiveObserving = true;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent_->SetButtonId(1);

    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) == context_->pointerItems.end());
}

/**
 * @tc.name: HandlePointerEvent_004
 * @tc.desc: when it's mouse right-button up event and isPerspectiveObserving is false, no send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseObservationToTouchHandlerTest, HandlePointerEvent_004, TestSize.Level1)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent_->SetButtonId(1);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    context_->isPerspectiveObserving = false;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) != context_->pointerItems.end());
}

/**
 * @tc.name: HandlePointerEvent_005
 * @tc.desc: when it's mouse right-button up event and currentPerspectiveObserving.mappingType is not
 * MOUSE_OBSERVATION_TO_TOUCH, no send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseObservationToTouchHandlerTest, HandlePointerEvent_005, TestSize.Level1)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent_->SetButtonId(1);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    context_->currentPerspectiveObserving.mappingType = OBSERVATION_KEY_TO_TOUCH;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) != context_->pointerItems.end());
}

/**
 * @tc.name: HandlePointerEvent_006
 * @tc.desc: when it's mouse event and current mouse's x is more than last mouse's x
 * and current mouse's y is more than last mouse's y, send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseObservationToTouchHandlerTest, HandlePointerEvent_006, TestSize.Level0)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent_->SetButtonId(1);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(MOUSE_X_VALUE + MOUSE_MOVE_DISTANCE,
                                                             MOUSE_Y_VALUE + MOUSE_MOVE_DISTANCE);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(context_->lastMousePointer.GetWindowX(), pointerItem.GetWindowX());
    ASSERT_EQ(context_->lastMousePointer.GetWindowY(), pointerItem.GetWindowY());
    ASSERT_EQ(handler_->touchEntity_.pointerId, OBSERVATION_POINT_ID);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE + X_STEP);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE + Y_STEP);

    /*
     * touch event's x cannot more than maxWidth
     * touch event's y cannot more than maxHeight
     */
    PointerEvent::PointerItem lastMovePoint = context_->pointerItems[OBSERVATION_POINT_ID];
    lastMovePoint.SetWindowX(MAX_WIDTH);
    lastMovePoint.SetWindowY(MAX_HEIGHT);
    context_->pointerItems[OBSERVATION_POINT_ID] = lastMovePoint;
    pointerEvent_->RemoveAllPointerItems();
    pointerItem = BuildPointerItem(MOUSE_X_VALUE + MOUSE_MOVE_DISTANCE + MOUSE_MOVE_DISTANCE,
                                   MOUSE_Y_VALUE + MOUSE_MOVE_DISTANCE + MOUSE_MOVE_DISTANCE);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);
    ASSERT_EQ(handler_->touchEntity_.xValue, MAX_WIDTH);
    ASSERT_EQ(handler_->touchEntity_.yValue, MAX_HEIGHT);
}

/**
 * @tc.name: HandlePointerEvent_007
 * @tc.desc: when it's mouse move event and current mouse's x is less than last mouse's x
 * and current mouse's y is less than last mouse's y, send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseObservationToTouchHandlerTest, HandlePointerEvent_007, TestSize.Level0)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent_->SetButtonId(1);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(MOUSE_X_VALUE - MOUSE_MOVE_DISTANCE,
                                                             MOUSE_Y_VALUE - MOUSE_MOVE_DISTANCE);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(context_->lastMousePointer.GetWindowX(), pointerItem.GetWindowX());
    ASSERT_EQ(context_->lastMousePointer.GetWindowY(), pointerItem.GetWindowY());
    ASSERT_EQ(handler_->touchEntity_.pointerId, OBSERVATION_POINT_ID);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE - X_STEP);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE - Y_STEP);

    /*
     * touch event's x cannot be less than MIN_EDGE
     * touch event's y cannot be less than MIN_EDGE
     */
    PointerEvent::PointerItem lastMovePoint = context_->pointerItems[OBSERVATION_POINT_ID];
    lastMovePoint.SetWindowX(0);
    lastMovePoint.SetWindowY(0);
    context_->pointerItems[OBSERVATION_POINT_ID] = lastMovePoint;
    pointerEvent_->RemoveAllPointerItems();
    pointerItem = BuildPointerItem(MOUSE_X_VALUE - MOUSE_MOVE_DISTANCE - MOUSE_MOVE_DISTANCE,
                                   MOUSE_Y_VALUE - MOUSE_MOVE_DISTANCE - MOUSE_MOVE_DISTANCE);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);
    ASSERT_EQ(handler_->touchEntity_.xValue, MIN_EDGE);
    ASSERT_EQ(handler_->touchEntity_.yValue, MIN_EDGE);
}

/**
 * @tc.name: HandlePointerEvent_008
 * @tc.desc: when it's mouse move event and current mouse's x is equal to last mouse's x
 * and current mouse's y is equal to last mouse's y, and current mouse's x is more than MIN_EDGE
 * , and current mouse's y is more than MIN_EDGE
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseObservationToTouchHandlerTest, HandlePointerEvent_008, TestSize.Level0)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent_->SetButtonId(1);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(MAX_WIDTH, MAX_HEIGHT);
    pointerEvent_->AddPointerItem(pointerItem);
    context_->lastMousePointer.SetWindowX(pointerItem.GetWindowX());
    context_->lastMousePointer.SetWindowY(pointerItem.GetWindowY());
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(context_->lastMousePointer.GetWindowX(), pointerItem.GetWindowX());
    ASSERT_EQ(context_->lastMousePointer.GetWindowY(), pointerItem.GetWindowY());
    ASSERT_EQ(handler_->touchEntity_.pointerId, OBSERVATION_POINT_ID);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE + X_STEP);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE + Y_STEP);

    /*
     * touch event's x cannot be more than maxWidth
     * touch event's y cannot be more than maxHeight
     */
    PointerEvent::PointerItem lastMovePoint = context_->pointerItems[OBSERVATION_POINT_ID];
    lastMovePoint.SetWindowX(MAX_WIDTH);
    lastMovePoint.SetWindowY(MAX_HEIGHT);
    context_->pointerItems[OBSERVATION_POINT_ID] = lastMovePoint;
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(handler_->touchEntity_.xValue, MAX_WIDTH);
    ASSERT_EQ(handler_->touchEntity_.yValue, MAX_HEIGHT);
}

/**
 * @tc.name: HandlePointerEvent_009
 * @tc.desc: when it's mouse move event and current mouse's x is equal to last mouse's x
 * and current mouse's y is equal to last mouse's y, and current mouse's x is less than or equal to MIN_EDGE
 * , and current mouse's y is less than or equal to MIN_EDGE
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseObservationToTouchHandlerTest, HandlePointerEvent_009, TestSize.Level0)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent_->SetButtonId(1);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(MIN_EDGE, MIN_EDGE);
    pointerEvent_->AddPointerItem(pointerItem);
    context_->lastMousePointer.SetWindowX(pointerItem.GetWindowX());
    context_->lastMousePointer.SetWindowY(pointerItem.GetWindowY());
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(context_->lastMousePointer.GetWindowX(), pointerItem.GetWindowX());
    ASSERT_EQ(context_->lastMousePointer.GetWindowY(), pointerItem.GetWindowY());
    ASSERT_EQ(handler_->touchEntity_.pointerId, OBSERVATION_POINT_ID);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE - X_STEP);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE - Y_STEP);

    /*
     * touch event's x cannot less than MIN_EDGE
     * touch event's y cannot less than MIN_EDGE
     */
    PointerEvent::PointerItem lastMovePoint = context_->pointerItems[OBSERVATION_POINT_ID];
    lastMovePoint.SetWindowX(MIN_EDGE);
    lastMovePoint.SetWindowY(MIN_EDGE);
    context_->pointerItems[OBSERVATION_POINT_ID] = lastMovePoint;
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(handler_->touchEntity_.xValue, MIN_EDGE);
    ASSERT_EQ(handler_->touchEntity_.yValue, MIN_EDGE);
}
}
}