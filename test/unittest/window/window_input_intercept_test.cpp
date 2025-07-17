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

#include "window_input_intercept.h"
#include "multi_modal_input_mgt_service_mock.h"
#include "pointer_event.h"

#undef private

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include "refbase.h"
#include "gamepad_event_callback_impl.h"
#include <input_manager.h>
#include <key_event.h>
#include <thread>
#include "input_event_client.h"

using ::testing::Return;
using namespace testing::ext;

namespace OHOS {
namespace GameController {
namespace {
const int64_t ACTION_TIME = 1111;
const int32_t DEVICE_ID = 12;
}

class WindowInputInterceptTest : public testing::Test {
public:
    void SetUp() override;

    void TearDown() override;
};

void WindowInputInterceptTest::SetUp()
{
    DelayedSingleton<WindowInputIntercept>::GetInstance()->consumer_ = nullptr;
    DelayedSingleton<WindowInputIntercept>::GetInstance()->registerDeviceIdSet_.clear();
}

void WindowInputInterceptTest::TearDown()
{
    DelayedSingleton<WindowInputIntercept>::GetInstance()->consumer_ = nullptr;
    DelayedSingleton<WindowInputIntercept>::GetInstance()->registerDeviceIdSet_.clear();
}

class WindowInputInterceptConsumerTest : public testing::Test {
public:
    void SetUp() override;

    void TearDown() override;

public:
    std::shared_ptr<MultiModalInputMgtServiceMock> multiModalInputMgtServiceMock_;
    std::shared_ptr<GamePadButtonEventCallback> buttonCallback_;
    std::shared_ptr<GamePadAxisEventCallback> axisCallback_;
    std::shared_ptr<WindowInputInterceptConsumer> consumer_;
};

void WindowInputInterceptConsumerTest::SetUp()
{
    multiModalInputMgtServiceMock_ = std::make_shared<MultiModalInputMgtServiceMock>();
    buttonCallback_ = std::make_shared<GamePadButtonEventCallback>();
    InputEventClient::RegisterGamePadButtonEventCallback(ApiTypeEnum::CAPI,
                                                         GamePadButtonTypeEnum::LeftShoulder,
                                                         buttonCallback_);

    axisCallback_ = std::make_shared<GamePadAxisEventCallback>();
    consumer_ = std::make_shared<WindowInputInterceptConsumer>();
}

void WindowInputInterceptConsumerTest::TearDown()
{
    InputEventClient::UnRegisterGamePadButtonEventCallback(ApiTypeEnum::CAPI,
                                                           GamePadButtonTypeEnum::LeftShoulder);
    InputEventClient::UnRegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                         GamePadAxisSourceTypeEnum::LeftThumbstick);
    InputEventClient::UnRegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                         GamePadAxisSourceTypeEnum::RightThumbstick);
    InputEventClient::UnRegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                         GamePadAxisSourceTypeEnum::LeftTriggerAxis);
    InputEventClient::UnRegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                         GamePadAxisSourceTypeEnum::RightTriggerAxis);
    InputEventClient::UnRegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                         GamePadAxisSourceTypeEnum::Dpad);
    multiModalInputMgtServiceMock_.reset();
    buttonCallback_ = nullptr;
    axisCallback_ = nullptr;
    consumer_ = nullptr;
    MultiModalInputMgtService::instance_ = nullptr;
}


/**
* @tc.name: RegisterWindowInputIntercept_001
* @tc.desc: After registration, deviceId is added to registerDeviceIdSet_,
 * and other deviceIds are added again. The value of consumer_ remains unchanged.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptTest, RegisterWindowInputIntercept_001, TestSize.Level0)
{
    int32_t deviceId = 2;
    DelayedSingleton<WindowInputIntercept>::GetInstance()->RegisterWindowInputIntercept(deviceId);
    ASSERT_TRUE(DelayedSingleton<WindowInputIntercept>::GetInstance()->registerDeviceIdSet_.count(deviceId));
    std::shared_ptr<Rosen::IInputEventInterceptConsumer> consumer
        = DelayedSingleton<WindowInputIntercept>::GetInstance()->consumer_;
    ASSERT_TRUE(consumer != nullptr);

    int32_t newDeviceId = 3;
    DelayedSingleton<WindowInputIntercept>::GetInstance()->RegisterWindowInputIntercept(newDeviceId);
    ASSERT_TRUE(DelayedSingleton<WindowInputIntercept>::GetInstance()->registerDeviceIdSet_.count(newDeviceId));
    ASSERT_TRUE(consumer == DelayedSingleton<WindowInputIntercept>::GetInstance()->consumer_);
}


/**
* @tc.name: UnRegisterWindowInputIntercept_001
* @tc.desc: After deregistration, deviceId is deleted from registerDeviceIdSet_.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptTest, UnRegisterWindowInputIntercept_001, TestSize.Level0)
{
    int32_t deviceId = 2;
    DelayedSingleton<WindowInputIntercept>::GetInstance()->RegisterWindowInputIntercept(deviceId);
    DelayedSingleton<WindowInputIntercept>::GetInstance()->UnRegisterWindowInputIntercept(deviceId);
    ASSERT_FALSE(DelayedSingleton<WindowInputIntercept>::GetInstance()->registerDeviceIdSet_.count(deviceId));
    ASSERT_TRUE(DelayedSingleton<WindowInputIntercept>::GetInstance()->consumer_ != nullptr);
}

/**
* @tc.name: UnRegisterAllWindowInputIntercept_001
* @tc.desc: After all registrations are canceled, all device IDs are deleted from registerDeviceIdSet_.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptTest, UnRegisterAllWindowInputIntercept_001, TestSize.Level0)
{
    int32_t deviceId = 2;
    DelayedSingleton<WindowInputIntercept>::GetInstance()->RegisterWindowInputIntercept(deviceId);
    DelayedSingleton<WindowInputIntercept>::GetInstance()->UnRegisterAllWindowInputIntercept();
    ASSERT_FALSE(DelayedSingleton<WindowInputIntercept>::GetInstance()->registerDeviceIdSet_.count(deviceId));
    ASSERT_TRUE(DelayedSingleton<WindowInputIntercept>::GetInstance()->consumer_ != nullptr);
}

static std::shared_ptr<MMI::KeyEvent> CreateNormalKeyEvent()
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    keyEvent->SetDeviceId(DEVICE_ID);
    keyEvent->SetKeyCode(GamePadButtonTypeEnum::LeftShoulder);
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    keyEvent->SetActionTime(ACTION_TIME);
    MMI::KeyEvent::KeyItem keyItem;
    keyItem.SetKeyCode(GamePadButtonTypeEnum::RightShoulder);
    keyItem.SetDeviceId(DEVICE_ID);
    keyItem.SetPressed(true);
    keyEvent->AddPressedKeyItems(keyItem);
    return keyEvent;
}

/**
* @tc.name: OnInputEvent_001
* @tc.desc: KeyEvent event: The LeftShoulder key lift event is received.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_001, TestSize.Level0)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = CreateNormalKeyEvent();
    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "test";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent->GetDeviceId())).WillOnce(
        Return(deviceInfo));
    consumer_->OnInputEvent(keyEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(buttonCallback_->result_.id, keyEvent->GetDeviceId());
    ASSERT_EQ(buttonCallback_->result_.keyCode, keyEvent->GetKeyCode());
    ASSERT_EQ(buttonCallback_->result_.keyCodeName, "LeftShoulder");
    ASSERT_EQ(buttonCallback_->result_.keyAction, 1);
    ASSERT_EQ(buttonCallback_->result_.uniq, deviceInfo.uniq);
    ASSERT_EQ(buttonCallback_->result_.keys.size(), 1);
    ASSERT_EQ(buttonCallback_->result_.keys[0].keyCode, GamePadButtonTypeEnum::RightShoulder);
    ASSERT_EQ(buttonCallback_->result_.keys[0].keyCodeName, "RightShoulder");
}

/**
* @tc.name: OnInputEvent_002
* @tc.desc: KeyEvent event: If an unknown action is received from the LeftShoulder key, the event is discarded.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_002, TestSize.Level0)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = CreateNormalKeyEvent();
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_CANCEL);

    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent->GetDeviceId())).Times(0);
    consumer_->OnInputEvent(keyEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_NE(buttonCallback_->result_.keyCode, keyEvent->GetKeyCode());
    EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(multiModalInputMgtServiceMock_.get()));
}

/**
* @tc.name: OnInputEvent_003
* @tc.desc: KeyEvent event: If an unknown key is received, the event is discarded.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_003, TestSize.Level0)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = CreateNormalKeyEvent();
    keyEvent->SetKeyCode(KeyEvent::KEYCODE_0);

    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent->GetDeviceId())).Times(0);
    consumer_->OnInputEvent(keyEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_NE(buttonCallback_->result_.keyCode, keyEvent->GetKeyCode());
    EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(multiModalInputMgtServiceMock_.get()));
}

/**
* @tc.name: OnInputEvent_004
* @tc.desc: KeyEvent event: If the device information queried based on deviceId is empty, the device is discarded.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_004, TestSize.Level0)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = CreateNormalKeyEvent();

    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent->GetDeviceId())).WillOnce(
        Return(deviceInfo));
    consumer_->OnInputEvent(keyEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_NE(buttonCallback_->result_.keyCode, keyEvent->GetKeyCode());
}

static std::shared_ptr<MMI::PointerEvent> CreateNormalPointerEvent()
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->SetDeviceId(DEVICE_ID);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_JOYSTICK);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_AXIS_BEGIN);
    pointerEvent->SetActionTime(ACTION_TIME);
    return pointerEvent;
}

/**
* @tc.name: OnInputEvent_005
* @tc.desc: PointerEvent event: Handles the axis event of the LeftThumbstick.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_005, TestSize.Level0)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = CreateNormalPointerEvent();
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_X, 1);
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_Y, 2);
    InputEventClient::RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                       GamePadAxisSourceTypeEnum::LeftThumbstick,
                                                       axisCallback_);
    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "test";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(pointerEvent->GetDeviceId())).WillOnce(
        Return(deviceInfo));
    consumer_->OnInputEvent(pointerEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(axisCallback_->result_.id, pointerEvent->GetDeviceId());
    ASSERT_EQ(axisCallback_->result_.uniq, deviceInfo.uniq);
    ASSERT_EQ(axisCallback_->result_.axisSourceType, GamePadAxisSourceTypeEnum::LeftThumbstick);
    ASSERT_EQ(axisCallback_->result_.xValue, 1);
    ASSERT_EQ(axisCallback_->result_.yValue, 2);
}

/**
* @tc.name: OnInputEvent_006
* @tc.desc: PointerEvent event: Handles the axis event of the RightThumbstick.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_006, TestSize.Level0)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = CreateNormalPointerEvent();
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_AXIS_UPDATE);
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_Z, 1);
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_RZ, 2);
    InputEventClient::RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                       GamePadAxisSourceTypeEnum::RightThumbstick,
                                                       axisCallback_);
    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "test";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(pointerEvent->GetDeviceId())).WillOnce(
        Return(deviceInfo));
    consumer_->OnInputEvent(pointerEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(axisCallback_->result_.id, pointerEvent->GetDeviceId());
    ASSERT_EQ(axisCallback_->result_.uniq, deviceInfo.uniq);
    ASSERT_EQ(axisCallback_->result_.axisSourceType, GamePadAxisSourceTypeEnum::RightThumbstick);
    ASSERT_EQ(axisCallback_->result_.zValue, 1);
    ASSERT_EQ(axisCallback_->result_.rzValue, 2);
}

/**
* @tc.name: OnInputEvent_007
* @tc.desc: PointerEvent event: Handles an axis event for LeftTriggerAxis
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_007, TestSize.Level0)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = CreateNormalPointerEvent();
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_AXIS_END);
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_BRAKE, 1);
    InputEventClient::RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                       GamePadAxisSourceTypeEnum::LeftTriggerAxis,
                                                       axisCallback_);
    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "test";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(pointerEvent->GetDeviceId())).WillOnce(
        Return(deviceInfo));
    consumer_->OnInputEvent(pointerEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(axisCallback_->result_.id, pointerEvent->GetDeviceId());
    ASSERT_EQ(axisCallback_->result_.uniq, deviceInfo.uniq);
    ASSERT_EQ(axisCallback_->result_.axisSourceType, GamePadAxisSourceTypeEnum::LeftTriggerAxis);
    ASSERT_EQ(axisCallback_->result_.brakeValue, 1);
}

/**
* @tc.name: OnInputEvent_008
* @tc.desc: PointerEvent event: Handles an axis event for RightTriggerAxis
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_008, TestSize.Level0)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = CreateNormalPointerEvent();
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_GAS, 1);
    InputEventClient::RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                       GamePadAxisSourceTypeEnum::RightTriggerAxis,
                                                       axisCallback_);
    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "test";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(pointerEvent->GetDeviceId())).WillOnce(
        Return(deviceInfo));
    consumer_->OnInputEvent(pointerEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(axisCallback_->result_.id, pointerEvent->GetDeviceId());
    ASSERT_EQ(axisCallback_->result_.uniq, deviceInfo.uniq);
    ASSERT_EQ(axisCallback_->result_.axisSourceType, GamePadAxisSourceTypeEnum::RightTriggerAxis);
    ASSERT_EQ(axisCallback_->result_.gasValue, 1);
}

/**
* @tc.name: OnInputEvent_009
* @tc.desc: PointerEvent event: Handles an axis event for Dpad
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_009, TestSize.Level0)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = CreateNormalPointerEvent();
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0X, 1);
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0Y, 2);
    InputEventClient::RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                       GamePadAxisSourceTypeEnum::Dpad,
                                                       axisCallback_);
    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "test";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(pointerEvent->GetDeviceId())).WillOnce(
        Return(deviceInfo));
    consumer_->OnInputEvent(pointerEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(axisCallback_->result_.id, pointerEvent->GetDeviceId());
    ASSERT_EQ(axisCallback_->result_.uniq, deviceInfo.uniq);
    ASSERT_EQ(axisCallback_->result_.axisSourceType, GamePadAxisSourceTypeEnum::Dpad);
    ASSERT_EQ(axisCallback_->result_.hatxValue, 1);
    ASSERT_EQ(axisCallback_->result_.hatyValue, 2);
}

/**
* @tc.name: OnInputEvent_010
* @tc.desc: PointerEvent event: not processed if sourceType is SOURCE_TYPE_TOUGHSCREEN
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_010, TestSize.Level0)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = CreateNormalPointerEvent();
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0X, 1);
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0Y, 2);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    InputEventClient::RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                       GamePadAxisSourceTypeEnum::Dpad,
                                                       axisCallback_);
    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "test";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(pointerEvent->GetDeviceId())).Times(0);
    consumer_->OnInputEvent(pointerEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(axisCallback_->result_.id, 0);
    EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(multiModalInputMgtServiceMock_.get()));
}

/**
* @tc.name: OnInputEvent_011
* @tc.desc: PointerEvent event: not processed if pointAction is POINTER_AXIS_ACTION_X
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_011, TestSize.Level0)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = CreateNormalPointerEvent();
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0X, 1);
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0Y, 2);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    InputEventClient::RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                       GamePadAxisSourceTypeEnum::Dpad,
                                                       axisCallback_);
    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "test";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(pointerEvent->GetDeviceId())).Times(0);
    consumer_->OnInputEvent(pointerEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(axisCallback_->result_.id, 0);
    EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(multiModalInputMgtServiceMock_.get()));
}

/**
* @tc.name: OnInputEvent_012
* @tc.desc: PointerEvent event: If the uniq of a device queried based on deviceId is empty, the device is discarded.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_012, TestSize.Level0)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = CreateNormalPointerEvent();
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0X, 1);
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0Y, 2);
    InputEventClient::RegisterGamePadAxisEventCallback(ApiTypeEnum::CAPI,
                                                       GamePadAxisSourceTypeEnum::Dpad,
                                                       axisCallback_);
    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(pointerEvent->GetDeviceId())).WillOnce(
        Return(deviceInfo));
    consumer_->OnInputEvent(pointerEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(axisCallback_->result_.id, 0);
}

/**
* @tc.name: OnInputEvent_013
* @tc.desc: KeyEvent event:If deviceId of the pressed key is different from that in keyEvent,
 * the pressed key is discarded.
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(WindowInputInterceptConsumerTest, OnInputEvent_013, TestSize.Level0)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = CreateNormalKeyEvent();
    keyEvent->SetDeviceId(0);
    MultiModalInputMgtService::instance_ = multiModalInputMgtServiceMock_;
    DeviceInfo deviceInfo;
    deviceInfo.uniq = "test";
    EXPECT_CALL(*(multiModalInputMgtServiceMock_.get()), GetDeviceInfo(keyEvent->GetDeviceId())).WillOnce(
        Return(deviceInfo));
    consumer_->OnInputEvent(keyEvent);
    ffrt::this_task::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(buttonCallback_->result_.id, keyEvent->GetDeviceId());
    ASSERT_EQ(buttonCallback_->result_.keyCode, keyEvent->GetKeyCode());
    ASSERT_EQ(buttonCallback_->result_.keyCodeName, "LeftShoulder");
    ASSERT_EQ(buttonCallback_->result_.keyAction, 1);
    ASSERT_EQ(buttonCallback_->result_.uniq, deviceInfo.uniq);
    ASSERT_EQ(buttonCallback_->result_.keys.size(), 0);
}

}
}