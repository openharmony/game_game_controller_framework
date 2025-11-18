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

#include "mouse_right_key_walking_to_touch_handler.h"

#undef private

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
namespace {
const int32_t X_VALUE = 539;
const int32_t Y_VALUE = 1001;
const int32_t MOUSE_X_VALUE = 1288;
const int32_t MOUSE_Y_VALUE = 825;
const int32_t RADIUS = 230;
const int32_t MAX_WIDTH = 2720;
const int32_t MAX_HEIGHT = 1260;
const int32_t SLEEP_TIME = 1500;
}

class MouseRightKeyWalkingToTouchHandlerEx : public MouseRightKeyWalkingToTouchHandler {
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

public:
    TouchEntity touchUpEntity_;
    TouchEntity touchDownEntity_;
    TouchEntity touchMoveEntity_;
    bool hasTouchEvent_{false};
};

class MouseRightKeyWalkingToTouchHandlerTest : public testing::Test {
public:
    void SetUp() override
    {
        handler_ = std::make_shared<MouseRightKeyWalkingToTouchHandlerEx>();
        context_ = std::make_shared<InputToTouchContext>();
        pointerEvent_ = PointerEvent::Create();
        pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
        pointerEvent_->SetButtonId(1);
        pointerItem_.SetWindowX(MOUSE_X_VALUE);
        pointerItem_.SetWindowY(MOUSE_Y_VALUE);
        pointerEvent_->AddPointerItem(pointerItem_);
        mappingInfo_ = BuildKeyToTouchMappingInfo();
        context_->windowInfoEntity.xCenter = MAX_WIDTH / HALF_LENGTH;
        context_->windowInfoEntity.yCenter = MAX_HEIGHT / HALF_LENGTH;
    }

    static KeyToTouchMappingInfo BuildKeyToTouchMappingInfo()
    {
        KeyToTouchMappingInfo info;
        info.mappingType = MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH;
        info.xValue = X_VALUE;
        info.yValue = Y_VALUE;
        info.radius = RADIUS;
        return info;
    }

    void TearDown() override
    {
        context_->ResetCurrentWalking();
    }

    int32_t SendMouseRightDownEvent()
    {
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
        handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);
        std::pair<bool, int32_t> pair = context_->GetPointerIdByKeyCode(KEY_CODE_WALK);
        int32_t pointerId = pair.second;
        return pointerId;
    }

public:
    std::shared_ptr<MouseRightKeyWalkingToTouchHandlerEx> handler_;
    std::shared_ptr<InputToTouchContext> context_;
    std::shared_ptr<MMI::PointerEvent> pointerEvent_;
    KeyToTouchMappingInfo mappingInfo_;
    PointerEvent::PointerItem pointerItem_;
};

/**
 * @tc.name: HandlePointerEvent_001
 * @tc.desc: when it's mouse right-button down event and isWalking is false
 * and hasDelayTask_ is false, send down and move touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyWalkingToTouchHandlerTest, HandlePointerEvent_001, TestSize.Level0)
{
    int32_t pointerId = SendMouseRightDownEvent();

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    ASSERT_TRUE(context_->isWalking);
    ASSERT_EQ(context_->currentWalking.mappingType, MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH);
    ASSERT_EQ(handler_->touchDownEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchDownEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(handler_->touchDownEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchDownEntity_.yValue, Y_VALUE);

    ASSERT_EQ(handler_->touchMoveEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchMoveEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchMoveEntity_.xValue, 459);
    ASSERT_EQ(handler_->touchMoveEntity_.yValue, 1216);
}

/**
 * @tc.name: HandlePointerEvent_002
 * @tc.desc: when it's mouse right-button down event and isWalking is true,
 * discard the event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyWalkingToTouchHandlerTest, HandlePointerEvent_002, TestSize.Level1)
{
    context_->isWalking = true;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);

    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_FALSE(handler_->hasTouchEvent_);
}

/**
 * @tc.name: HandlePointerEvent_003
 * @tc.desc: when it's mouse right-button up event and isWalking is true,
 * and currentWalking.mappingType is MOUSE_RIGHT_KEY_WALKING_TO_TOUCH
 * and delayTime is 0, send up touch event immediately
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyWalkingToTouchHandlerTest, HandlePointerEvent_003, TestSize.Level1)
{
    int32_t pointerId = SendMouseRightDownEvent();

    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_FALSE(DelayedSingleton<MouseRightKeyWalkingDelayHandleTask>::GetInstance()->hasDelayTask_);
    ASSERT_TRUE(context_->pointerItems.find(pointerId) == context_->pointerItems.end());
    ASSERT_FALSE(context_->isWalking);
    ASSERT_EQ(context_->currentWalking.mappingType, 0);
    ASSERT_EQ(handler_->touchUpEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchUpEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchUpEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchUpEntity_.yValue, Y_VALUE);
}

/**
 * @tc.name: HandlePointerEvent_004
 * @tc.desc: when it's mouse right-button up event and isWalking is false,
 * discard the event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyWalkingToTouchHandlerTest, HandlePointerEvent_004, TestSize.Level1)
{
    int32_t pointerId = SendMouseRightDownEvent();
    context_->isWalking = false;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    ASSERT_EQ(context_->currentWalking.mappingType, MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH);
}

/**
 * @tc.name: HandlePointerEvent_005
 * @tc.desc: when it's mouse right-button up event and isWalking is true
 * and currentWalking.mappingType is MOUSE_RIGHT_KEY_WALKING_TO_TOUCH,
 * discard the event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyWalkingToTouchHandlerTest, HandlePointerEvent_005, TestSize.Level1)
{
    int32_t pointerId = SendMouseRightDownEvent();

    context_->currentWalking.mappingType = MappingTypeEnum::DPAD_KEY_TO_TOUCH;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    ASSERT_EQ(context_->currentWalking.mappingType, MappingTypeEnum::DPAD_KEY_TO_TOUCH);
}

/**
 * @tc.name: HandlePointerEvent_006
 * @tc.desc: when it's mouse move event and isWalking is false
 * and currentWalking.mappingType is MOUSE_RIGHT_KEY_WALKING_TO_TOUCH,
 * send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyWalkingToTouchHandlerTest, HandlePointerEvent_006, TestSize.Level0)
{
    int32_t pointerId = SendMouseRightDownEvent();
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    ASSERT_EQ(handler_->touchMoveEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchMoveEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchMoveEntity_.xValue, 459);
    ASSERT_EQ(handler_->touchMoveEntity_.yValue, 1216);
}

/**
 * @tc.name: HandlePointerEvent_007
 * @tc.desc: when it's mouse move event and isWalking is true
 * and currentWalking.mappingType is not MOUSE_RIGHT_KEY_WALKING_TO_TOUCH,
 * send touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyWalkingToTouchHandlerTest, HandlePointerEvent_007, TestSize.Level0)
{
    context_->isWalking = false;
    context_->currentWalking = mappingInfo_;
    context_->currentWalking.mappingType = DPAD_KEY_TO_TOUCH;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);

    ASSERT_FALSE(handler_->hasTouchEvent_);
}

/**
 * @tc.name: HandlePointerEvent_008
 * @tc.desc: when it's mouse right-button up event and isWalking is true,
 * and currentWalking.mappingType is MOUSE_RIGHT_KEY_WALKING_TO_TOUCH
 * and delayTime is 1, delay sending touch up event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyWalkingToTouchHandlerTest, HandlePointerEvent_008, TestSize.Level1)
{
    int32_t pointerId = SendMouseRightDownEvent();
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);

    handler_->HandlePointerEvent(context_, pointerEvent_, mappingInfo_);
    ASSERT_FALSE(context_->isWalking);
    ASSERT_EQ(context_->currentWalking.mappingType, 0);
    ASSERT_TRUE(DelayedSingleton<MouseRightKeyWalkingDelayHandleTask>::GetInstance()->hasDelayTask_);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
    ASSERT_FALSE(DelayedSingleton<MouseRightKeyWalkingDelayHandleTask>::GetInstance()->hasDelayTask_);
    ASSERT_TRUE(context_->pointerItems.find(pointerId) == context_->pointerItems.end());
    ASSERT_EQ(handler_->touchUpEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchUpEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchUpEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchUpEntity_.yValue, Y_VALUE);
}

/**
 * @tc.name: HandlePointerEvent_009
 * @tc.desc: when it's mouse right-button down event and isWalking is false
 * and hasDelayTask_ is true, send up and down and move touch event
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(MouseRightKeyWalkingToTouchHandlerTest, HandlePointerEvent_009, TestSize.Level0)
{
    DelayedSingleton<MouseRightKeyWalkingDelayHandleTask>::GetInstance()->hasDelayTask_ = true;
    int32_t applyPointerId = DelayedSingleton<PointerManager>::GetInstance()->ApplyPointerId();
    PointerEvent::PointerItem pointerItem;
    context_->pointerItems[applyPointerId] = pointerItem;
    pointerItem.SetWindowX(X_VALUE);
    pointerItem.SetWindowY(Y_VALUE);
    context_->pointerIdWithKeyCodeMap[KEY_CODE_WALK] = applyPointerId;
    DelayedSingleton<MouseRightKeyWalkingDelayHandleTask>::GetInstance()->context_ = context_;

    int32_t pointerId = SendMouseRightDownEvent();

    ASSERT_TRUE(context_->pointerItems.find(pointerId) != context_->pointerItems.end());
    ASSERT_TRUE(context_->isWalking);
    ASSERT_EQ(context_->currentWalking.mappingType, MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH);
    ASSERT_EQ(handler_->touchUpEntity_.pointerId, applyPointerId);
    ASSERT_EQ(handler_->touchUpEntity_.pointerAction, PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(handler_->touchUpEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchUpEntity_.yValue, Y_VALUE);

    ASSERT_EQ(handler_->touchDownEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchDownEntity_.pointerAction, PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(handler_->touchDownEntity_.xValue, X_VALUE);
    ASSERT_EQ(handler_->touchDownEntity_.yValue, Y_VALUE);

    ASSERT_EQ(handler_->touchMoveEntity_.pointerId, pointerId);
    ASSERT_EQ(handler_->touchMoveEntity_.pointerAction, PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(handler_->touchMoveEntity_.xValue, 459);
    ASSERT_EQ(handler_->touchMoveEntity_.yValue, 1216);
}
}
}