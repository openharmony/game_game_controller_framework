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
#include "observation_key_to_touch_handler.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
namespace {
const int32_t X_VALUE = 2045;
const int32_t Y_VALUE = 863;
const int32_t KEY_CODE = 2301;
const int32_t MOUSE_X_VALUE = 2235;
const int32_t MOUSE_Y_VALUE = 650;
const int32_t MAX_WIDTH = 2720;
const int32_t MAX_HEIGHT = 1260;
const int32_t X_STEP = 1;
const int32_t Y_STEP = 1;
const int32_t MOUSE_MOVE_DISTANCE = 10;
}
class ObservationKeyToTouchHandlerEx : public ObservationKeyToTouchHandler {
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
        ObservationKeyToTouchHandler::HandleKeyDown(context, keyEvent, mappingInfo, deviceInfo);
    }

    void HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                     const std::shared_ptr<MMI::KeyEvent> &keyEvent, const DeviceInfo &deviceInfo) override
    {
        ObservationKeyToTouchHandler::HandleKeyUp(context, keyEvent, deviceInfo);
    }

public:
    TouchEntity touchEntity_;
};

class ObservationKeyToTouchHandlerTest : public testing::Test {
public:
    void SetUp()
    {
        handler_ = std::make_shared<ObservationKeyToTouchHandlerEx>();
        context_ = std::make_shared<InputToTouchContext>();
        pointerEvent_ = PointerEvent::Create();
        pointerItem_.SetWindowX(MOUSE_X_VALUE);
        pointerItem_.SetWindowY(MOUSE_Y_VALUE);
        pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
        pointerEvent_->AddPointerItem(pointerItem_);
        mappingInfo_ = BuildKeyToTouchMappingInfo();
        context_->windowInfoEntity.maxWidth = MAX_WIDTH;
        context_->windowInfoEntity.maxHeight = MAX_HEIGHT;

        keyEvent_ = KeyEvent::Create();
        keyEvent_->SetKeyCode(KEY_CODE);
    }

    KeyToTouchMappingInfo BuildKeyToTouchMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.keyCode = KEY_CODE;
        info.mappingType = MappingTypeEnum::OBSERVATION_KEY_TO_TOUCH;
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
    std::shared_ptr<ObservationKeyToTouchHandlerEx> handler_;
    std::shared_ptr<InputToTouchContext> context_;
    std::shared_ptr<MMI::PointerEvent> pointerEvent_;
    std::shared_ptr<MMI::KeyEvent> keyEvent_;
    KeyToTouchMappingInfo mappingInfo_;
    PointerEvent::PointerItem pointerItem_;
    DeviceInfo deviceInfo_;
};

/**
 * @tc.name: HandleKeyDown_001
 * @tc.desc: when key is down and context_->isPerspectiveObserving is false, can send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ObservationKeyToTouchHandlerTest, HandleKeyDown_001, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
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
}

/**
 * @tc.name: HandleKeyDown_002
 * @tc.desc: when key is down and context_->isPerspectiveObserving is true, cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ObservationKeyToTouchHandlerTest, HandleKeyDown_002, TestSize.Level1)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    context_->isPerspectiveObserving = true;
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) == context_->pointerItems.end());
    ASSERT_EQ(handler_->touchEntity_.xValue, 0);
    ASSERT_EQ(handler_->touchEntity_.yValue, 0);
}

/**
 * @tc.name: HandleKeyUp_001
 * @tc.desc: when key is up and context_->isPerspectiveObserving is true, can send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ObservationKeyToTouchHandlerTest, HandleKeyUp_001, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    context_->lastMousePointer.SetWindowX(X_VALUE);
    context_->lastMousePointer.SetWindowY(Y_VALUE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_FALSE(context_->isPerspectiveObserving);

    ASSERT_EQ(context_->currentPerspectiveObserving.mappingType, 0);
    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) == context_->pointerItems.end());
    ASSERT_EQ(context_->lastMousePointer.GetWindowX(), 0);
    ASSERT_EQ(context_->lastMousePointer.GetWindowY(), 0);
    ASSERT_EQ(handler_->touchEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->touchEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchEntity_.pointerId, OBSERVATION_POINT_ID);
}

/**
 * @tc.name: HandleKeyUp_002
 * @tc.desc: when key is up and context_->isPerspectiveObserving is false, cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ObservationKeyToTouchHandlerTest, HandleKeyUp_002, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    context_->isPerspectiveObserving = false;
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) != context_->pointerItems.end());
}

/**
 * @tc.name: HandleKeyUp_003
 * @tc.desc: when key is up and context_->isPerspectiveObserving is true and keycode is not same with template,
 * cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ObservationKeyToTouchHandlerTest, HandleKeyUp_003, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

    keyEvent_->SetKeyCode(0);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_TRUE(context_->isPerspectiveObserving);
    ASSERT_TRUE(context_->pointerItems.find(OBSERVATION_POINT_ID) != context_->pointerItems.end());
}

/**
 * @tc.name: HandlePointerEvent_001
 * @tc.desc: when it's mouse event and current mouse's x is more than last mouse's x
 * and current mouse's y is more than last mouse's y, send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ObservationKeyToTouchHandlerTest, HandlePointerEvent_001, TestSize.Level0)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);

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
}
}