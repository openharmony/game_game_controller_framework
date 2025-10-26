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

#define private public

#include "crosshair_key_to_touch_handler.h"

#undef private

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
class CrosshairKeyToTouchHandlerEx : public CrosshairKeyToTouchHandler {
public:
    void BuildAndSendPointerEvent(std::shared_ptr<InputToTouchContext> &context_,
                                  const TouchEntity &touchEntity) override
    {
        hasTouchEvent_ = true;
        BaseKeyToTouchHandler::BuildAndSendPointerEvent(context_, touchEntity);
        if (touchEntity.pointerAction == PointerEvent::POINTER_ACTION_UP) {
            touchUpEntity_ = touchEntity;
        }
        if (touchEntity.pointerAction == PointerEvent::POINTER_ACTION_DOWN) {
            touchDownEntity_ = touchEntity;
        }
        if (touchEntity.pointerAction == PointerEvent::POINTER_ACTION_MOVE) {
            touchMoveEntity_ = touchEntity;
        }
    }

    void HandleKeyDown(std::shared_ptr<InputToTouchContext> &context,
                       const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                       const KeyToTouchMappingInfo &mappingInfo,
                       const DeviceInfo &deviceInfo) override
    {
        CrosshairKeyToTouchHandler::HandleKeyDown(context, keyEvent, mappingInfo, deviceInfo);
    }

    void HandleKeyUp(std::shared_ptr<InputToTouchContext> &context,
                     const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                     const DeviceInfo &deviceInfo) override
    {
        CrosshairKeyToTouchHandler::HandleKeyUp(context, keyEvent, deviceInfo);
    }

public:
    TouchEntity touchUpEntity_;
    TouchEntity touchDownEntity_;
    TouchEntity touchMoveEntity_;
    bool hasTouchEvent_{false};
};

class CrosshairKeyToTouchHandlerTest : public testing::Test {
public:
    void SetUp() override
    {
        handler_ = std::make_shared<CrosshairKeyToTouchHandlerEx>();
        context_ = std::make_shared<InputToTouchContext>();
        pointerEvent_ = PointerEvent::Create();
        pointerEvent_->SetButtonId(1);
        pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
        pointerItem_.SetWindowX(MOUSE_X_VALUE);
        pointerItem_.SetWindowY(MOUSE_Y_VALUE);
        pointerEvent_->AddPointerItem(pointerItem_);
        mappingInfo_ = BuildKeyToTouchMappingInfo();
        context_->windowInfoEntity.maxWidth = MAX_WIDTH;
        context_->windowInfoEntity.maxHeight = MAX_HEIGHT;
        keyEvent_ = KeyEvent::Create();
        keyEvent_->SetKeyCode(KEY_CODE);
    }

    void TearDown() override
    {
        context_->ResetCurrentCrosshairInfo();
    }

    static KeyToTouchMappingInfo BuildKeyToTouchMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.mappingType = MappingTypeEnum::CROSSHAIR_KEY_TO_TOUCH;
        info.xValue = X_VALUE;
        info.yValue = Y_VALUE;
        info.xStep = X_STEP;
        info.yStep = Y_STEP;
        info.keyCode = KEY_CODE;
        return info;
    }

    static PointerEvent::PointerItem BuildPointerItem(int32_t xVal, int32_t yVal)
    {
        PointerEvent::PointerItem pointerItem;
        pointerItem.SetWindowX(xVal);
        pointerItem.SetWindowY(yVal);
        return pointerItem;
    }

    int32_t SendTouchDown()
    {
        keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
        handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);
        std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_CROSSHAIR);
        int32_t pointerId = pair.second;
        return pointerId;
    }

public:
    std::shared_ptr<CrosshairKeyToTouchHandlerEx> handler_;
    std::shared_ptr<InputToTouchContext> context_;
    std::shared_ptr<MMI::PointerEvent> pointerEvent_;
    std::shared_ptr<MMI::KeyEvent> keyEvent_;
    KeyToTouchMappingInfo mappingInfo_;
    PointerEvent::PointerItem pointerItem_;
    DeviceInfo deviceInfo_;
};

/**
 * @tc.name: HandleKeyDown_001
 * @tc.desc: when key is down and context_->isCrosshairMode is false,
 * send down touch event, and system is into isCrosshairMode
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandleKeyDown_001, TestSize.Level0)
{
    int32_t pointerId = SendTouchDown();

    ASSERT_TRUE(context_->isCrosshairMode);
    ASSERT_FALSE(context_->isEnterCrosshairInfo);
    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    ASSERT_EQ(context_->currentCrosshairInfo.mappingType, mappingInfo_.mappingType);
    ASSERT_EQ(handler_->touchDownEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchDownEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(handler_->touchDownEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchDownEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->currentIdx_, 1);
}

/**
 * @tc.name: HandleKeyDown_002
 * @tc.desc: when key is down and context_->isCrosshairMode is true, discard the event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandleKeyDown_002, TestSize.Level1)
{
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    context_->isCrosshairMode = true;
    handler_->HandleKeyDown(context_, keyEvent_, mappingInfo_, deviceInfo_);
    ASSERT_TRUE(context_->isCrosshairMode);
    ASSERT_EQ(context_->currentCrosshairInfo.mappingType, 0);
}

/**
 * @tc.name: HandleKeyUp_001
 * @tc.desc: when key is up and context_->isCrosshairMode is true, and context_->isEnterCrosshairInfo is false.
 * context_->isEnterCrosshairInfo will be change to true,
 *  no send up touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandleKeyUp_001, TestSize.Level0)
{
    context_->isCrosshairMode = true;
    context_->isEnterCrosshairInfo = false;
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_TRUE(context_->isEnterCrosshairInfo);
    ASSERT_FALSE(handler_->hasTouchEvent_);
}

/**
 * @tc.name: HandleKeyUp_002
 * @tc.desc: when key is up and context_->isCrosshairMode is true, and context_->isEnterCrosshairInfo is true.
 * exit the crosshairMode. if pointerId is in pointerItems, send up touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandleKeyUp_002, TestSize.Level0)
{
    // first key up, it will enter into crosshair status
    int32_t pointerId = SendTouchDown();
    ASSERT_TRUE(context_->isCrosshairMode);
    ASSERT_FALSE(context_->isEnterCrosshairInfo);

    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);
    ASSERT_TRUE(context_->isCrosshairMode);
    ASSERT_TRUE(context_->isEnterCrosshairInfo);

    // second key up, it will exit crosshair status
    context_->pointerItems[pointerId] = BuildPointerItem(X_VALUE, Y_VALUE);
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) == context_->pointerItems.end());
    ASSERT_FALSE(context_->isCrosshairMode);
    ASSERT_FALSE(context_->isEnterCrosshairInfo);
    ASSERT_EQ(context_->currentCrosshairInfo.mappingType, 0);
    ASSERT_EQ(handler_->touchUpEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchUpEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchUpEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->currentIdx_, 0);
}

/**
 * @tc.name: HandleKeyUp_003
 * @tc.desc: when key is up and context_->isCrosshairMode is true, and context_->isEnterCrosshairInfo is true.
 * exit the crosshairMode. if CROSSHAIR_POINT_ID is not in pointerItems, cannot send up touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandleKeyUp_003, TestSize.Level0)
{
    context_->isCrosshairMode = true;
    context_->isEnterCrosshairInfo = true;
    context_->currentCrosshairInfo = mappingInfo_;
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    ASSERT_FALSE(context_->isCrosshairMode);
    ASSERT_FALSE(context_->isEnterCrosshairInfo);
    ASSERT_EQ(context_->currentCrosshairInfo.mappingType, 0);
    ASSERT_FALSE(handler_->hasTouchEvent_);
}

/**
 * @tc.name: HandleKeyUp_004
 * @tc.desc: when key is up and context_->isCrosshairMode is false，cannot send touch command
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandleKeyUp_004, TestSize.Level0)
{
    context_->isCrosshairMode = false;
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);
    std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_CROSSHAIR);
    ASSERT_FALSE(pair.first);
    ASSERT_FALSE(context_->isEnterCrosshairInfo);
    ASSERT_FALSE(handler_->hasTouchEvent_);
}

/**
 * @tc.name: HandlePointerEvent_001
 * @tc.desc: when it's mouse move event and context->isEnterCrosshairInfo is true,
 * and not move to x's edge, it will only send move event;
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandlePointerEvent_001, TestSize.Level0)
{
    int32_t pointerId = SendTouchDown();
    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    handler_->HandleKeyUp(context_, keyEvent_, deviceInfo_);

    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(MOUSE_X_VALUE + MOUSE_MOVE_DISTANCE,
                                                             MOUSE_Y_VALUE + MOUSE_MOVE_DISTANCE);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);
    ASSERT_EQ(handler_->touchDownEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchDownEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);

    ASSERT_EQ(context_->lastMousePointer.GetWindowX(), pointerItem.GetWindowX());
    ASSERT_EQ(context_->lastMousePointer.GetWindowY(), pointerItem.GetWindowY());
    ASSERT_EQ(handler_->touchMoveEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchMoveEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchMoveEntity_.xValue, X_VALUE + X_STEP);
    ASSERT_EQ(handler_->touchMoveEntity_.yValue, Y_VALUE + Y_STEP);

    ASSERT_EQ(handler_->touchUpEntity_.pointerId, 0);
    ASSERT_EQ(handler_->touchUpEntity_.pointerAction, 0);
}

/**
 * @tc.name: HandlePointerEvent_002
 * @tc.desc: when it's mouse move event and context->isEnterCrosshairInfo is false, cannot send touch event;
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandlePointerEvent_002, TestSize.Level0)
{
    context_->isEnterCrosshairInfo = false;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(MOUSE_X_VALUE + MOUSE_MOVE_DISTANCE,
                                                             MOUSE_Y_VALUE + MOUSE_MOVE_DISTANCE);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_FALSE(handler_->hasTouchEvent_);
}

/**
 * @tc.name: HandlePointerEvent_003
 * @tc.desc: when it's not mouse move event and context->isEnterCrosshairInfo is true, cannot send touch event;
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandlePointerEvent_003, TestSize.Level0)
{
    context_->isEnterCrosshairInfo = true;

    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(MOUSE_X_VALUE + MOUSE_MOVE_DISTANCE,
                                                             MOUSE_Y_VALUE + MOUSE_MOVE_DISTANCE);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_FALSE(handler_->hasTouchEvent_);
}

/**
 * @tc.name: HandlePointerEvent_004
 * @tc.desc: when it's mouse move event and context->isEnterCrosshairInfo is true,
 * and move to x's max edge, it will send move event;
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandlePointerEvent_004, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentCrosshairInfo(mappingInfo_, pointerId);
    handler_->currentIdx_ = 1;
    PointerEvent::PointerItem lastMovePoint;
    lastMovePoint.SetWindowX(MAX_WIDTH);
    lastMovePoint.SetWindowY(MAX_HEIGHT);
    context_->pointerItems[pointerId] = lastMovePoint;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(MAX_WIDTH,
                                                             MAX_HEIGHT);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(context_->lastMousePointer.GetWindowX(), MAX_WIDTH);
    ASSERT_EQ(context_->lastMousePointer.GetWindowY(), MAX_HEIGHT);
    ASSERT_EQ(handler_->touchMoveEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchMoveEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchMoveEntity_.xValue, MAX_WIDTH);
    ASSERT_EQ(handler_->touchMoveEntity_.yValue, MAX_HEIGHT);
}

/**
 * @tc.name: HandlePointerEvent_005
 * @tc.desc: when it's mouse move event and context->isEnterCrosshairInfo is true,
 * and move to x's min edge, it will send move event;
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandlePointerEvent_005, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentCrosshairInfo(mappingInfo_, pointerId);
    handler_->currentIdx_ = 1;
    PointerEvent::PointerItem lastMovePoint;
    lastMovePoint.SetWindowX(0);
    lastMovePoint.SetWindowY(0);
    context_->pointerItems[pointerId] = lastMovePoint;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(0, 0);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(context_->lastMousePointer.GetWindowX(), 0);
    ASSERT_EQ(context_->lastMousePointer.GetWindowY(), 0);
    ASSERT_EQ(handler_->touchMoveEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchMoveEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchMoveEntity_.xValue, 1);
    ASSERT_EQ(handler_->touchMoveEntity_.yValue, 1);
}

/**
 * @tc.name: HandlePointerEvent_006
 * @tc.desc: when it's mouse move event and context->isEnterCrosshairInfo is true,
 * and the current position of mouse pointer is equal to the last position, mouse is not moving to the edge
 * so send move event that xValue and yValue are same with last move point.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandlePointerEvent_006, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentCrosshairInfo(mappingInfo_, pointerId);
    handler_->currentIdx_ = 1;
    int32_t nextIdx = handler_->currentIdx_ + 1;
    PointerEvent::PointerItem lastMovePoint;
    lastMovePoint.SetWindowX(X_VALUE - X_STEP);
    lastMovePoint.SetWindowY(Y_VALUE - Y_STEP);
    context_->pointerItems[pointerId] = lastMovePoint;
    context_->lastMousePointer = BuildPointerItem(X_VALUE, Y_VALUE);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(X_VALUE, Y_VALUE);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(context_->lastMousePointer.GetWindowX(), X_VALUE);
    ASSERT_EQ(context_->lastMousePointer.GetWindowY(), Y_VALUE);
    ASSERT_EQ(handler_->touchMoveEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchMoveEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchMoveEntity_.xValue, X_VALUE - X_STEP);
    ASSERT_EQ(handler_->touchMoveEntity_.yValue, Y_VALUE - Y_STEP);
    ASSERT_EQ(handler_->currentIdx_, nextIdx);
}

/**
 * @tc.name: HandlePointerEvent_007
 * @tc.desc: when it's mouse move event and context->isEnterCrosshairInfo is true,
 * and the current position of mouse pointer is equal to the last position, mouse is moving to the edge
 * so send move event that xValue and yValue are same not with last move point.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandlePointerEvent_007, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentCrosshairInfo(mappingInfo_, pointerId);
    handler_->currentIdx_ = 1;
    int32_t nextIdx = handler_->currentIdx_ + 1;
    PointerEvent::PointerItem lastMovePoint;
    lastMovePoint.SetWindowX(X_VALUE);
    lastMovePoint.SetWindowY(Y_VALUE);
    context_->pointerItems[pointerId] = lastMovePoint;
    context_->lastMousePointer = BuildPointerItem(MAX_WIDTH - 1, 1);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(MAX_WIDTH - 1, 1);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(handler_->touchMoveEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchMoveEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchMoveEntity_.xValue, X_VALUE + X_STEP);
    ASSERT_EQ(handler_->touchMoveEntity_.yValue, Y_VALUE - Y_STEP);
    ASSERT_EQ(handler_->currentIdx_, nextIdx);
}

/**
 * @tc.name: HandlePointerEvent_008
 * @tc.desc: when it's mouse move event and context->isEnterCrosshairInfo is true,
 * and handler_->currentIdx_ is 0, then send down event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandlePointerEvent_008, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentCrosshairInfo(mappingInfo_, pointerId);
    handler_->currentIdx_ = 0;
    int32_t nextIdx = handler_->currentIdx_ + 1;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(X_VALUE, Y_VALUE);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(handler_->touchDownEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchDownEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(handler_->touchDownEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchDownEntity_.yValue, Y_VALUE);
    ASSERT_EQ(handler_->currentIdx_, nextIdx);
}

/**
 * @tc.name: HandlePointerEvent_009
 * @tc.desc: when it's mouse move event and context->isEnterCrosshairInfo is true,
 * and handler_->currentIdx_ is 10, then send up event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(CrosshairKeyToTouchHandlerTest, HandlePointerEvent_009, TestSize.Level0)
{
    int32_t pointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    context_->SetCurrentCrosshairInfo(mappingInfo_, pointerId);
    handler_->currentIdx_ = 10;
    PointerEvent::PointerItem lastMovePoint;
    lastMovePoint.SetWindowX(X_VALUE - X_STEP);
    lastMovePoint.SetWindowY(Y_VALUE - Y_STEP);
    context_->pointerItems[pointerId] = lastMovePoint;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->RemoveAllPointerItems();
    PointerEvent::PointerItem pointerItem = BuildPointerItem(X_VALUE, Y_VALUE);
    pointerEvent_->AddPointerItem(pointerItem);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_EQ(handler_->touchUpEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchUpEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchUpEntity_.xValue, lastMovePoint.GetWindowX());
    ASSERT_EQ(handler_->touchUpEntity_.yValue, lastMovePoint.GetWindowY());
    ASSERT_EQ(handler_->currentIdx_, 0);
}
}
}