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

#define private public

#include "input_event_callback.h"

#undef private

#include <gtest/gtest.h>
#include "refbase.h"
#include "gamepad_event_callback_impl.h"

using ::testing::Return;
using namespace testing::ext;

namespace OHOS {
namespace GameController {

class InputEventCallbackTest : public testing::Test {
public:
    void SetUp() override;

    void TearDown() override;
};

void InputEventCallbackTest::SetUp()
{
    DelayedSingleton<InputEventCallback>::GetInstance()->gamePadButtonCallback_.clear();
    DelayedSingleton<InputEventCallback>::GetInstance()->gamePadAxisCallback_.clear();
}

void InputEventCallbackTest::TearDown()
{
    DelayedSingleton<InputEventCallback>::GetInstance()->gamePadButtonCallback_.clear();
    DelayedSingleton<InputEventCallback>::GetInstance()->gamePadAxisCallback_.clear();
}

/**
* @tc.name: OnGamePadButtonEventCallback_001
* @tc.desc: If a callback method exists, the callback is successful.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(InputEventCallbackTest, OnGamePadButtonEventCallback_001, TestSize.Level0)
{
    std::shared_ptr<GamePadButtonEventCallback> callback = std::make_shared<GamePadButtonEventCallback>();
    DelayedSingleton<InputEventCallback>::GetInstance()
        ->RegisterGamePadButtonEventCallback(ApiTypeEnum::CAPI,
                                             GamePadButtonTypeEnum::RightThumbstickButton,
                                             callback);
    GamePadButtonEvent buttonEvent;
    buttonEvent.id = 11;
    buttonEvent.keyCodeName = "RightThumbstickButton";
    buttonEvent.keyCode = GamePadButtonTypeEnum::RightThumbstickButton;
    DelayedSingleton<InputEventCallback>::GetInstance()->OnGamePadButtonEventCallback(buttonEvent);
    ASSERT_EQ(buttonEvent.id, callback->result_.id);
    ASSERT_EQ(buttonEvent.keyCodeName, callback->result_.keyCodeName);
    ASSERT_EQ(buttonEvent.keyCode, callback->result_.keyCode);
}

/**
* @tc.name: OnGamePadButtonEventCallback_002
* @tc.desc: If there is no callback method, the event is discarded.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(InputEventCallbackTest, OnGamePadButtonEventCallback_002, TestSize.Level0)
{
    std::shared_ptr<GamePadButtonEventCallback> callback = std::make_shared<GamePadButtonEventCallback>();
    DelayedSingleton<InputEventCallback>::GetInstance()
        ->RegisterGamePadButtonEventCallback(ApiTypeEnum::CAPI,
                                             GamePadButtonTypeEnum::LeftThumbstickButton,
                                             callback);
    GamePadButtonEvent buttonEvent;
    buttonEvent.id = 11;
    buttonEvent.keyCodeName = "RightThumbstickButton";
    buttonEvent.keyCode = GamePadButtonTypeEnum::RightThumbstickButton;
    DelayedSingleton<InputEventCallback>::GetInstance()->OnGamePadButtonEventCallback(buttonEvent);
    ASSERT_EQ(0, callback->result_.keyCode);
}

/**
* @tc.name: OnGamePadAxisEventCallback_001
* @tc.desc: If a callback method exists, the callback is successful.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(InputEventCallbackTest, OnGamePadAxisEventCallback_001, TestSize.Level0)
{
    std::shared_ptr<GamePadAxisEventCallback> callback = std::make_shared<GamePadAxisEventCallback>();
    DelayedSingleton<InputEventCallback>::GetInstance()
        ->RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                           GamePadAxisSourceTypeEnum::RightThumbstick,
                                           callback);
    GamePadAxisEvent axisEvent;
    axisEvent.gasValue = 1.0;
    axisEvent.axisSourceType = GamePadAxisSourceTypeEnum::RightThumbstick;
    DelayedSingleton<InputEventCallback>::GetInstance()->OnGamePadAxisEventCallback(axisEvent);

    ASSERT_EQ(axisEvent.gasValue, callback->result_.gasValue);
    ASSERT_EQ(axisEvent.axisSourceType, callback->result_.axisSourceType);
}

/**
* @tc.name: OnGamePadAxisEventCallback_002
* @tc.desc: If no callback method exists, the event is discarded.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(InputEventCallbackTest, OnGamePadAxisEventCallback_002, TestSize.Level0)
{
    std::shared_ptr<GamePadAxisEventCallback> callback = std::make_shared<GamePadAxisEventCallback>();
    DelayedSingleton<InputEventCallback>::GetInstance()
        ->RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                           GamePadAxisSourceTypeEnum::LeftThumbstick,
                                           callback);
    GamePadAxisEvent axisEvent;
    axisEvent.gasValue = 1.0;
    axisEvent.axisSourceType = GamePadAxisSourceTypeEnum::RightThumbstick;
    DelayedSingleton<InputEventCallback>::GetInstance()->OnGamePadAxisEventCallback(axisEvent);

    ASSERT_EQ(0.0, callback->result_.gasValue);
}

}
}