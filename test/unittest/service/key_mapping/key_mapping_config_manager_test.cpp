/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/hwext/gtest-ext.h"
#include "gtest/hwext/gtest-tag.h"
#include "gmock/gmock-actions.h"
#include "gmock/gmock-spec-builders.h"

#define private public

#include "key_mapping_config_manager.h"
#include <gamecontroller_errors.h>

#undef private

#include <gtest/gtest.h>
#include <string>
#include "refbase.h"

using namespace testing::ext;
using namespace std;
using json = nlohmann::json;

namespace OHOS {
namespace GameController {
namespace {
const std::string BUNDLE_NAME = "test.pkg";
const std::string BUNDLE_NAME_2 = "newtest.pkg";

const int32_t CUSTOM_KEYCODE_A = 12;
const int32_t CUSTOM_KEYCODE_B = 13;
const int32_t CUSTOM_KEYCODE_A_X_VALUE = 14;
const int32_t CUSTOM_KEYCODE_A_Y_VALUE = 15;
const int32_t CUSTOM_KEYCODE_B_X_VALUE = 16;
const int32_t CUSTOM_KEYCODE_B_Y_VALUE = 17;
const int32_t CUSTOM_KEYCODE_C_X_VALUE = 18;
const int32_t CUSTOM_KEYCODE_C_Y_VALUE = 19;

const int32_t DEFAULT_KEYCODE_C = 121;
const int32_t DEFAULT_KEYCODE_D = 131;
const int32_t DEFAULT_KEYCODE_D_X_VALUE = 141;
const int32_t DEFAULT_KEYCODE_D_Y_VALUE = 151;
const int32_t DEFAULT_KEYCODE_E_X_VALUE = 161;
const int32_t DEFAULT_KEYCODE_E_Y_VALUE = 171;
const int32_t DEFAULT_KEYCODE_F_X_VALUE = 181;
const int32_t DEFAULT_KEYCODE_F_Y_VALUE = 191;

const int32_t RADIUS = 1000;
const int32_t SKILL_RANGE = 1001;
const int32_t X_STEP = 1002;
const int32_t Y_STEP = 1003;
const int32_t UP = 1004;
const int32_t DOWN = 1005;
const int32_t LEFT = 1006;
const int32_t RIGHT = 1007;
const int32_t COMBINATION_1 = 1008;
const int32_t COMBINATION_2 = 1009;
const int32_t DELAY_TIME = 5;
const int32_t X_VALUE = 1400;
const int32_t Y_VALUE = 1500;
const int32_t KEY_MAPPING_SIZE = 4;

}

class KeyMappingConfigManagerTest : public testing::Test {
public:
    void SetUp();

    void TearDown();

    void ClearCache();
};

void KeyMappingConfigManagerTest::SetUp()
{
    ClearCache();
}

void KeyMappingConfigManagerTest::TearDown()
{
    ClearCache();
}

void KeyMappingConfigManagerTest::ClearCache()
{
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->defaultKeyMappingInfoConfigMap_.clear();
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->customKeyMappingInfoConfigMap_.clear();
}

static KeyToTouchMappingInfo BuildMouseRightWalking()
{
    KeyToTouchMappingInfo keyMapping;
    keyMapping.mappingType = MOUSE_RIGHT_KEY_WALKING_TO_TOUCH;
    keyMapping.delayTime = DELAY_TIME;
    keyMapping.yValue = Y_VALUE;
    keyMapping.xValue = X_VALUE;
    keyMapping.radius = RADIUS;
    return keyMapping;
}

static KeyToTouchMappingInfo BuildSingleKeyMapping(int32_t keyCode, int32_t xValue, int32_t yValue)
{
    KeyToTouchMappingInfo keyMapping;
    keyMapping.mappingType = SINGE_KEY_TO_TOUCH;
    keyMapping.keyCode = keyCode;
    keyMapping.yValue = xValue;
    keyMapping.xValue = yValue;
    return keyMapping;
}

static KeyToTouchMappingInfo BuildSkillKeyMapping(int32_t keyCode, int32_t xValue, int32_t yValue)
{
    KeyToTouchMappingInfo keyMapping;
    keyMapping.mappingType = SKILL_KEY_TO_TOUCH;
    keyMapping.keyCode = keyCode;
    keyMapping.yValue = xValue;
    keyMapping.xValue = yValue;
    keyMapping.radius = RADIUS;
    keyMapping.skillRange = SKILL_RANGE;
    return keyMapping;
}

static KeyToTouchMappingInfo BuildCombinationKeyMapping(int32_t xValue, int32_t yValue)
{
    KeyToTouchMappingInfo keyMapping;
    keyMapping.mappingType = COMBINATION_KEY_TO_TOUCH;
    keyMapping.yValue = xValue;
    keyMapping.xValue = yValue;
    keyMapping.combinationKeys.push_back(COMBINATION_1);
    keyMapping.combinationKeys.push_back(COMBINATION_2);
    return keyMapping;
}

static KeyToTouchMappingInfo BuildKeyboardObservation(int32_t xValue, int32_t yValue)
{
    KeyToTouchMappingInfo keyMapping;
    keyMapping.mappingType = KEY_BOARD_OBSERVATION_TO_TOUCH;
    keyMapping.yValue = xValue;
    keyMapping.xValue = yValue;
    DpadKeyCodeEntity dpadKeyCodeEntity;
    dpadKeyCodeEntity.up = UP;
    dpadKeyCodeEntity.down = DOWN;
    dpadKeyCodeEntity.right = RIGHT;
    dpadKeyCodeEntity.left = LEFT;
    keyMapping.dpadKeyCodeEntity = dpadKeyCodeEntity;
    keyMapping.xStep = X_STEP;
    keyMapping.yStep = Y_STEP;
    return keyMapping;
}

static GameKeyMappingInfo BuildCustomKeyMappingConfig()
{
    GameKeyMappingInfo keyMappingInfoConfig;
    keyMappingInfoConfig.bundleName = BUNDLE_NAME;
    keyMappingInfoConfig.deviceType = GAME_KEY_BOARD;
    keyMappingInfoConfig.customKeyToTouchMappings.push_back(
        BuildSkillKeyMapping(CUSTOM_KEYCODE_A, CUSTOM_KEYCODE_A_X_VALUE, CUSTOM_KEYCODE_A_Y_VALUE));
    keyMappingInfoConfig.customKeyToTouchMappings.push_back(
        BuildCombinationKeyMapping(CUSTOM_KEYCODE_B_X_VALUE, CUSTOM_KEYCODE_B_Y_VALUE));
    keyMappingInfoConfig.customKeyToTouchMappings.push_back(
        BuildKeyboardObservation(CUSTOM_KEYCODE_C_X_VALUE, CUSTOM_KEYCODE_C_Y_VALUE));
    keyMappingInfoConfig.customKeyToTouchMappings.push_back(BuildMouseRightWalking());
    return keyMappingInfoConfig;
}

static GameKeyMappingInfo BuildDefaultKeyMappingConfig()
{
    GameKeyMappingInfo keyMappingInfoConfig;
    keyMappingInfoConfig.bundleName = BUNDLE_NAME;
    keyMappingInfoConfig.deviceType = GAME_KEY_BOARD;
    keyMappingInfoConfig.defaultKeyToTouchMappings.push_back(
        BuildSkillKeyMapping(DEFAULT_KEYCODE_C, DEFAULT_KEYCODE_D_X_VALUE, DEFAULT_KEYCODE_D_Y_VALUE));
    keyMappingInfoConfig.defaultKeyToTouchMappings.push_back(
        BuildCombinationKeyMapping(DEFAULT_KEYCODE_E_X_VALUE, DEFAULT_KEYCODE_E_Y_VALUE));
    keyMappingInfoConfig.defaultKeyToTouchMappings.push_back(
        BuildKeyboardObservation(DEFAULT_KEYCODE_F_X_VALUE, DEFAULT_KEYCODE_F_Y_VALUE));
    keyMappingInfoConfig.defaultKeyToTouchMappings.push_back(BuildMouseRightWalking());
    return keyMappingInfoConfig;
}

static void CheckKeyMapping(const std::vector<KeyToTouchMappingInfo> &exceptConfig,
                            const std::vector<KeyMapping> &resultConfig)
{
    ASSERT_EQ(exceptConfig.size(), resultConfig.size());
    for (size_t i = 0; i < exceptConfig.size(); ++i) {
        KeyToTouchMappingInfo exceptMapping = exceptConfig[i];
        KeyMapping realMapping = resultConfig[i];
        ASSERT_EQ(exceptMapping.mappingType, realMapping.mappingType);
        ASSERT_EQ(exceptMapping.keyCode, realMapping.keyCode);
        ASSERT_EQ(exceptMapping.xValue, realMapping.xValue);
        ASSERT_EQ(exceptMapping.yValue, realMapping.yValue);
        ASSERT_EQ(exceptMapping.radius, realMapping.radius);
        ASSERT_EQ(exceptMapping.skillRange, realMapping.skillRange);
        ASSERT_EQ(exceptMapping.xStep, realMapping.xStep);
        ASSERT_EQ(exceptMapping.yStep, realMapping.yStep);
        ASSERT_EQ(exceptMapping.combinationKeys.size(), realMapping.combinationKeys.size());
        int32_t size = static_cast<int32_t>(exceptMapping.combinationKeys.size());
        if (!realMapping.combinationKeys.empty()) {
            for (auto idx = 0; idx < size; idx++) {
                ASSERT_EQ(exceptMapping.combinationKeys[idx], realMapping.combinationKeys[idx]);
            }
        }
        ASSERT_EQ(exceptMapping.dpadKeyCodeEntity.up, realMapping.dpadInfo.up);
        ASSERT_EQ(exceptMapping.dpadKeyCodeEntity.down, realMapping.dpadInfo.down);
        ASSERT_EQ(exceptMapping.dpadKeyCodeEntity.left, realMapping.dpadInfo.left);
        ASSERT_EQ(exceptMapping.dpadKeyCodeEntity.right, realMapping.dpadInfo.right);
        ASSERT_EQ(exceptMapping.yStep, realMapping.yStep);
        ASSERT_EQ(exceptMapping.delayTime, realMapping.delayTime);
    }
}

/**
 * @tc.name: SetCustomGameKeyMappingConfig_001
 * @tc.desc: Succeeded in setting the customized configuration.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingConfigManagerTest, SetCustomGameKeyMappingConfig_001, TestSize.Level0)
{
    GameKeyMappingInfo keyMappingInfoConfig = BuildCustomKeyMappingConfig();

    int32_t rtn = DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(
        keyMappingInfoConfig);

    ASSERT_EQ(GAME_CONTROLLER_SUCCESS, rtn);
    ASSERT_EQ(1, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->customKeyMappingInfoConfigMap_.size());
    KeyMappingInfoConfig result = DelayedSingleton<KeyMappingConfigManager>::GetInstance()
        ->customKeyMappingInfoConfigMap_.at(keyMappingInfoConfig.bundleName + "_"
        + std::to_string(keyMappingInfoConfig.deviceType));
    ASSERT_EQ(keyMappingInfoConfig.bundleName, result.bundleName);
    ASSERT_EQ(GAME_KEY_BOARD, result.deviceType);
    ASSERT_EQ(KEY_MAPPING_SIZE, result.customKeyMappings.size());
    CheckKeyMapping(keyMappingInfoConfig.customKeyToTouchMappings, result.customKeyMappings);
}

/**
 * @tc.name: SetCustomGameKeyMappingConfig_002
 * @tc.desc: Succeeded in deleting the specified customized configuration.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingConfigManagerTest, SetCustomGameKeyMappingConfig_002, TestSize.Level0)
{
    // add config
    GameKeyMappingInfo keyMappingInfoConfig = BuildCustomKeyMappingConfig();
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(
        keyMappingInfoConfig);
    GameKeyMappingInfo keyMappingInfoConfigNew = BuildCustomKeyMappingConfig();
    keyMappingInfoConfigNew.bundleName = BUNDLE_NAME_2;
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(
        keyMappingInfoConfigNew);
    ASSERT_EQ(2, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->customKeyMappingInfoConfigMap_.size());

    // delete config
    keyMappingInfoConfig.isOprDelete = true;
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(
        keyMappingInfoConfig);

    // check delete result
    ASSERT_EQ(1, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->customKeyMappingInfoConfigMap_.size());
    KeyMappingInfoConfig result = DelayedSingleton<KeyMappingConfigManager>::GetInstance()
        ->customKeyMappingInfoConfigMap_.at(keyMappingInfoConfigNew.bundleName + "_"
        + std::to_string(keyMappingInfoConfigNew.deviceType));
    ASSERT_EQ(keyMappingInfoConfigNew.deviceType, result.deviceType);
}

/**
 * @tc.name: SetCustomGameKeyMappingConfig_003
 * @tc.desc: Succeeded in deleting the customized configuration by bundleName.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingConfigManagerTest, SetCustomGameKeyMappingConfig_003, TestSize.Level0)
{
    // add config
    GameKeyMappingInfo keyMappingInfoConfig = BuildCustomKeyMappingConfig();
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(
        keyMappingInfoConfig);
    GameKeyMappingInfo keyMappingInfoConfigNew;
    keyMappingInfoConfigNew.deviceType = HOVER_TOUCH_PAD;
    keyMappingInfoConfigNew.bundleName = BUNDLE_NAME;
    keyMappingInfoConfigNew.customKeyToTouchMappings.push_back(
        BuildSingleKeyMapping(CUSTOM_KEYCODE_B, CUSTOM_KEYCODE_B_X_VALUE, CUSTOM_KEYCODE_B_Y_VALUE));
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(
        keyMappingInfoConfigNew);
    ASSERT_EQ(2, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->customKeyMappingInfoConfigMap_.size());

    // delete config
    keyMappingInfoConfig.isOprDelete = true;
    keyMappingInfoConfig.deviceType = UNKNOWN;
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(
        keyMappingInfoConfig);

    // check delete result
    ASSERT_EQ(0, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->customKeyMappingInfoConfigMap_.size());
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->LoadConfigFromJsonFile();
    ASSERT_EQ(0, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->customKeyMappingInfoConfigMap_.size());
}

/**
 * @tc.name: SetCustomGameKeyMappingConfig_004
 * @tc.desc: Param is invalid
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingConfigManagerTest, SetCustomGameKeyMappingConfig_004, TestSize.Level0)
{
    GameKeyMappingInfo keyMappingInfoConfig = BuildCustomKeyMappingConfig();
    keyMappingInfoConfig.bundleName = "";

    int32_t rtn = DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(
        keyMappingInfoConfig);

    ASSERT_EQ(GAME_ERR_ARGUMENT_INVALID, rtn);
}

/**
 * @tc.name: SetDefaultGameKeyMappingConfig_001
 * @tc.desc: Succeeded in setting the default configuration.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingConfigManagerTest, SetDefaultGameKeyMappingConfig_001, TestSize.Level0)
{
    GameKeyMappingInfo keyMappingInfoConfig = BuildDefaultKeyMappingConfig();

    int32_t rtn = DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(
        keyMappingInfoConfig);

    ASSERT_EQ(GAME_CONTROLLER_SUCCESS, rtn);
    ASSERT_EQ(1, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->defaultKeyMappingInfoConfigMap_.size());
    KeyMappingInfoConfig result = DelayedSingleton<KeyMappingConfigManager>::GetInstance()
        ->defaultKeyMappingInfoConfigMap_.at(BUNDLE_NAME + "_" + std::to_string(keyMappingInfoConfig.deviceType));
    ASSERT_EQ(keyMappingInfoConfig.bundleName, result.bundleName);
    ASSERT_EQ(GAME_KEY_BOARD, result.deviceType);
    ASSERT_EQ(KEY_MAPPING_SIZE, result.defaultKeyMappings.size());
    CheckKeyMapping(keyMappingInfoConfig.defaultKeyToTouchMappings, result.defaultKeyMappings);
}

/**
 * @tc.name: SetDefaultGameKeyMappingConfig_002
 * @tc.desc: Succeeded in deleting the default configuration by bundleName and deviceType.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingConfigManagerTest, SetDefaultGameKeyMappingConfig_002, TestSize.Level0)
{
    // add config
    GameKeyMappingInfo keyMappingInfoConfig = BuildDefaultKeyMappingConfig();
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(
        keyMappingInfoConfig);
    GameKeyMappingInfo keyMappingInfoConfigNew = BuildDefaultKeyMappingConfig();
    keyMappingInfoConfigNew.bundleName = BUNDLE_NAME_2;
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(
        keyMappingInfoConfigNew);
    ASSERT_EQ(2, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->defaultKeyMappingInfoConfigMap_.size());

    // delete config
    keyMappingInfoConfig.isOprDelete = true;
    int32_t rtn = DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(
        keyMappingInfoConfig);

    // check delete result
    ASSERT_EQ(GAME_CONTROLLER_SUCCESS, rtn);
    ASSERT_EQ(1, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->defaultKeyMappingInfoConfigMap_.size());
    KeyMappingInfoConfig result = DelayedSingleton<KeyMappingConfigManager>::GetInstance()
        ->defaultKeyMappingInfoConfigMap_.at(keyMappingInfoConfigNew.bundleName + "_3");
    ASSERT_EQ(GAME_KEY_BOARD, result.deviceType);
}

/**
 * @tc.name: SetDefaultGameKeyMappingConfig_003
 * @tc.desc: Succeeded in deleting the default configuration by bundleName.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingConfigManagerTest, SetDefaultGameKeyMappingConfig_003, TestSize.Level0)
{
    // add config
    GameKeyMappingInfo keyMappingInfoConfig = BuildDefaultKeyMappingConfig();
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(
        keyMappingInfoConfig);
    GameKeyMappingInfo keyMappingInfoConfigNew;
    keyMappingInfoConfigNew.deviceType = HOVER_TOUCH_PAD;
    keyMappingInfoConfigNew.bundleName = BUNDLE_NAME;
    keyMappingInfoConfigNew.customKeyToTouchMappings.push_back(
        BuildSingleKeyMapping(DEFAULT_KEYCODE_D, DEFAULT_KEYCODE_D_X_VALUE, DEFAULT_KEYCODE_D_Y_VALUE));
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(
        keyMappingInfoConfigNew);

    // delete config
    keyMappingInfoConfig.isOprDelete = true;
    keyMappingInfoConfig.deviceType = UNKNOWN;
    int32_t rtn = DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(
        keyMappingInfoConfig);

    // check delete result
    ASSERT_EQ(GAME_CONTROLLER_SUCCESS, rtn);
    ASSERT_EQ(0, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->defaultKeyMappingInfoConfigMap_.size());
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->LoadConfigFromJsonFile();
    ASSERT_EQ(0, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->defaultKeyMappingInfoConfigMap_.size());
}

/**
 * @tc.name: SetDefaultGameKeyMappingConfig_004
 * @tc.desc: Param is invalid
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingConfigManagerTest, SetDefaultGameKeyMappingConfig_004, TestSize.Level0)
{
    GameKeyMappingInfo keyMappingInfoConfig = BuildDefaultKeyMappingConfig();
    keyMappingInfoConfig.bundleName = "";

    int32_t rtn = DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(
        keyMappingInfoConfig);

    ASSERT_EQ(GAME_ERR_ARGUMENT_INVALID, rtn);
}

/**
 * @tc.name: LoadConfigFromJsonFile_001
 * @tc.desc: Loading System Configurations and Custom Configurations
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingConfigManagerTest, LoadConfigFromJsonFile_001, TestSize.Level0)
{
    // add data
    GameKeyMappingInfo defaultKeyMappingInfoConfig = BuildDefaultKeyMappingConfig();
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(
        defaultKeyMappingInfoConfig);
    GameKeyMappingInfo customKeyMappingInfoConfig = BuildCustomKeyMappingConfig();
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(
        customKeyMappingInfoConfig);
    this->ClearCache();

    // load data
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->LoadConfigFromJsonFile();

    // check data
    ASSERT_EQ(1, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->defaultKeyMappingInfoConfigMap_.size());
    KeyMappingInfoConfig defaultResult = DelayedSingleton<KeyMappingConfigManager>::GetInstance()
        ->defaultKeyMappingInfoConfigMap_.at(defaultKeyMappingInfoConfig.bundleName + "_3");
    ASSERT_EQ(KEY_MAPPING_SIZE, defaultResult.defaultKeyMappings.size());
    CheckKeyMapping(defaultKeyMappingInfoConfig.defaultKeyToTouchMappings, defaultResult.defaultKeyMappings);

    ASSERT_EQ(1, DelayedSingleton<KeyMappingConfigManager>::GetInstance()->customKeyMappingInfoConfigMap_.size());
    KeyMappingInfoConfig customResult = DelayedSingleton<KeyMappingConfigManager>::GetInstance()
        ->customKeyMappingInfoConfigMap_.at(
        customKeyMappingInfoConfig.bundleName + "_" + std::to_string(customKeyMappingInfoConfig.deviceType));
    ASSERT_EQ(KEY_MAPPING_SIZE, customResult.customKeyMappings.size());
    CheckKeyMapping(customKeyMappingInfoConfig.customKeyToTouchMappings, customResult.customKeyMappings);
}

/**
 * @tc.name: GetGameKeyMappingConfig_001
 * @tc.desc: Only query default config by bundleName and deviceType
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingConfigManagerTest, GetGameKeyMappingConfig_001, TestSize.Level1)
{
    // add data
    GameKeyMappingInfo defaultKeyMappingInfoConfig = BuildDefaultKeyMappingConfig();
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(
        defaultKeyMappingInfoConfig);
    GameKeyMappingInfo customKeyMappingInfoConfig = BuildCustomKeyMappingConfig();
    customKeyMappingInfoConfig.deviceType = HOVER_TOUCH_PAD;
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(customKeyMappingInfoConfig);

    // get data
    GetGameKeyMappingInfoParam param;
    param.bundleName = defaultKeyMappingInfoConfig.bundleName;
    param.deviceType = GAME_KEY_BOARD;
    GameKeyMappingInfo gameKeyMappingInfo;
    int32_t result = DelayedSingleton<KeyMappingConfigManager>::GetInstance()
        ->GetGameKeyMappingConfig(param, gameKeyMappingInfo);

    // check data
    ASSERT_EQ(GAME_CONTROLLER_SUCCESS, result);
    ASSERT_EQ(KEY_MAPPING_SIZE, gameKeyMappingInfo.defaultKeyToTouchMappings.size());
    ASSERT_EQ(0, gameKeyMappingInfo.customKeyToTouchMappings.size());
    KeyMappingInfoConfig defaultConfig = DelayedSingleton<KeyMappingConfigManager>::GetInstance()
        ->defaultKeyMappingInfoConfigMap_.at(param.bundleName + "_3");
    CheckKeyMapping(gameKeyMappingInfo.defaultKeyToTouchMappings, defaultConfig.defaultKeyMappings);
}

/**
 * @tc.name: GetGameKeyMappingConfig_002
 * @tc.desc: Both query default and custom config
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingConfigManagerTest, GetGameKeyMappingConfig_002, TestSize.Level0)
{
    // add data
    GameKeyMappingInfo defaultKeyMappingInfoConfig = BuildDefaultKeyMappingConfig();
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(
        defaultKeyMappingInfoConfig);
    GameKeyMappingInfo customKeyMappingInfoConfig = BuildCustomKeyMappingConfig();
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(customKeyMappingInfoConfig);

    // get data
    GetGameKeyMappingInfoParam param;
    param.bundleName = customKeyMappingInfoConfig.bundleName;
    param.deviceType = GAME_KEY_BOARD;
    GameKeyMappingInfo gameKeyMappingInfo;
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->GetGameKeyMappingConfig(param, gameKeyMappingInfo);

    // check data
    ASSERT_EQ(KEY_MAPPING_SIZE, gameKeyMappingInfo.defaultKeyToTouchMappings.size());
    ASSERT_EQ(KEY_MAPPING_SIZE, gameKeyMappingInfo.customKeyToTouchMappings.size());
    KeyMappingInfoConfig customConfig = DelayedSingleton<KeyMappingConfigManager>::GetInstance()
        ->customKeyMappingInfoConfigMap_.at(param.bundleName + "_" + std::to_string(param.deviceType));
    CheckKeyMapping(gameKeyMappingInfo.customKeyToTouchMappings, customConfig.customKeyMappings);
}

/**
 * @tc.name: GetGameKeyMappingConfig_003
 * @tc.desc: check param is invalid
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(KeyMappingConfigManagerTest, GetGameKeyMappingConfig_003, TestSize.Level0)
{
    // add data
    GameKeyMappingInfo defaultKeyMappingInfoConfig = BuildDefaultKeyMappingConfig();
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(
        defaultKeyMappingInfoConfig);
    GameKeyMappingInfo customKeyMappingInfoConfig = BuildCustomKeyMappingConfig();
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(customKeyMappingInfoConfig);

    // get data
    GetGameKeyMappingInfoParam param;
    param.bundleName = "";
    param.deviceType = customKeyMappingInfoConfig.deviceType;
    GameKeyMappingInfo gameKeyMappingInfo;
    int32_t result = DelayedSingleton<KeyMappingConfigManager>::GetInstance()
        ->GetGameKeyMappingConfig(param, gameKeyMappingInfo);

    // check data
    ASSERT_EQ(GAME_ERR_ARGUMENT_INVALID, result);
}
}
}