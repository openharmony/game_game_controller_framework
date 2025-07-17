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
#include "input_event_client.h"
#include "gamepad_event_callback_impl.h"

using ::testing::Return;
using namespace testing::ext;

namespace OHOS {
namespace GameController {

class InputEventClientTest : public testing::Test {
public:
    void SetUp() override;

    void TearDown() override;
};

void InputEventClientTest::SetUp()
{
    DelayedSingleton<InputEventCallback>::GetInstance()->gamePadButtonCallback_.clear();
    DelayedSingleton<InputEventCallback>::GetInstance()->gamePadAxisCallback_.clear();
}

void InputEventClientTest::TearDown()
{
    DelayedSingleton<InputEventCallback>::GetInstance()->gamePadButtonCallback_.clear();
    DelayedSingleton<InputEventCallback>::GetInstance()->gamePadAxisCallback_.clear();
}

/**
* @tc.name: RegisterGamePadButtonEventCallback_001
* @tc.desc: If the value of callback is nullptr, the system discards the callback and does not process the callback.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(InputEventClientTest, RegisterGamePadButtonEventCallback_001, TestSize.Level0)
{
    InputEventClient::RegisterGamePadButtonEventCallback(ApiTypeEnum::CAPI,
                                                         GamePadButtonTypeEnum::RightThumbstickButton,
                                                         nullptr);
    ASSERT_EQ(0, DelayedSingleton<InputEventCallback>::GetInstance()->gamePadButtonCallback_.size());
}

/**
* @tc.name: RegisterGamePadButtonEventCallback_002
* @tc.desc: If the value of callback is not nullptr, the data is cached to gamePadButtonCallback_.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(InputEventClientTest, RegisterGamePadButtonEventCallback_002, TestSize.Level0)
{
    std::shared_ptr<GamePadButtonCallbackBase> callback = std::make_shared<GamePadButtonEventCallback>();
    InputEventClient::RegisterGamePadButtonEventCallback(ApiTypeEnum::CAPI,
                                                         GamePadButtonTypeEnum::RightThumbstickButton,
                                                         callback);
    ASSERT_EQ(1, DelayedSingleton<InputEventCallback>::GetInstance()->gamePadButtonCallback_.size());
    std::shared_ptr<GamePadButtonCallbackBase> buttonCallback = DelayedSingleton<InputEventCallback>::GetInstance()
        ->gamePadButtonCallback_[GamePadButtonTypeEnum::RightThumbstickButton];
    ASSERT_EQ(callback, buttonCallback);
}

/**
* @tc.name: UnRegisterGamePadButtonEventCallback_001
* @tc.desc: If the value of callback is not nullptr, the data is cached to gamePadButtonCallback_.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(InputEventClientTest, UnRegisterGamePadButtonEventCallback_001, TestSize.Level0)
{
    std::shared_ptr<GamePadButtonCallbackBase> callback = std::make_shared<GamePadButtonEventCallback>();
    InputEventClient::RegisterGamePadButtonEventCallback(ApiTypeEnum::CAPI,
                                                         GamePadButtonTypeEnum::RightThumbstickButton,
                                                         callback);
    InputEventClient::UnRegisterGamePadButtonEventCallback(ApiTypeEnum::CAPI,
                                                           GamePadButtonTypeEnum::RightThumbstickButton);
    ASSERT_EQ(0, DelayedSingleton<InputEventCallback>::GetInstance()->gamePadButtonCallback_.size());
}

/**
* @tc.name: RegisterGamePadAxisEventCallback_001
* @tc.desc: If the value of callback is nullptr, the system discards the callback and does not process the callback.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(InputEventClientTest, RegisterGamePadAxisEventCallback_001, TestSize.Level0)
{
    InputEventClient::RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                       GamePadAxisSourceTypeEnum::LeftTriggerAxis,
                                                       nullptr);
    ASSERT_EQ(0, DelayedSingleton<InputEventCallback>::GetInstance()->gamePadAxisCallback_.size());
}

/**
* @tc.name: RegisterGamePadAxisEventCallback_002
* @tc.desc: If the value of callback is not nullptr, the callback is cached to gamePadAxisCallback_.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(InputEventClientTest, RegisterGamePadAxisEventCallback_002, TestSize.Level0)
{
    std::shared_ptr<GamePadAxisCallbackBase> callback = std::make_shared<GamePadAxisEventCallback>();
    InputEventClient::RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                       GamePadAxisSourceTypeEnum::LeftTriggerAxis,
                                                       callback);
    ASSERT_EQ(1, DelayedSingleton<InputEventCallback>::GetInstance()->gamePadAxisCallback_.size());
    std::shared_ptr<GamePadAxisCallbackBase> axisEventCallback = DelayedSingleton<InputEventCallback>::GetInstance()
        ->gamePadAxisCallback_[GamePadAxisSourceTypeEnum::LeftTriggerAxis];
    ASSERT_EQ(callback, axisEventCallback);
}

/**
* @tc.name: UnRegisterGamePadAxisEventCallback_001
* @tc.desc: Callback for deleting the corresponding axis from the gamePadAxisCallback_ cache
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(InputEventClientTest, UnRegisterGamePadAxisEventCallback_001, TestSize.Level0)
{
    std::shared_ptr<GamePadAxisCallbackBase> callback = std::make_shared<GamePadAxisEventCallback>();
    InputEventClient::RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                       GamePadAxisSourceTypeEnum::LeftTriggerAxis,
                                                       callback);
    InputEventClient::UnRegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                         GamePadAxisSourceTypeEnum::LeftTriggerAxis);
    ASSERT_EQ(0, DelayedSingleton<InputEventCallback>::GetInstance()->gamePadAxisCallback_.size());
}
}
}