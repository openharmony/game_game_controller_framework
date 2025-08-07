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
#include "gamecontroller_keymapping_model.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {

namespace {
const int32_t MAX_SINGLE_KEY_MAPPING_SIZE = 50;
const int32_t MAX_COMBINATION_KEY_MAPPING_SIZE = 30;
const int32_t MAX_SKILL_KEY_MAPPING_SIZE = 10;
const int32_t DPAD_UP = 2012;
const int32_t DPAD_DOWN = 2013;
const int32_t DPAD_LEFT = 2014;
const int32_t DPAD_RIGHT = 2015;
const int32_t KEYBOARD_OBSERVATION_UP = 2039;
const int32_t KEYBOARD_OBSERVATION_DOWN = 2035;
const int32_t KEYBOARD_OBSERVATION_LEFT = 2017;
const int32_t KEYBOARD_OBSERVATION_RIGHT = 2020;
const size_t MAX_SINGLE_KEY_SIZE_FOR_HOVER_TOUCH_PAD = 2;
}

class GameControllerKeymappingModeTest : public testing::Test {
public:
    void SetUp() override;

    void TearDown() override;
};

void GameControllerKeymappingModeTest::SetUp()
{
}

void GameControllerKeymappingModeTest::TearDown()
{
}

static KeyToTouchMappingInfo BuildKeyMapping(int32_t keyCode, int32_t xValue, int32_t yValue)
{
    KeyToTouchMappingInfo keyMapping;
    keyMapping.mappingType = SINGE_KEY_TO_TOUCH;
    keyMapping.keyCode = keyCode;
    keyMapping.yValue = xValue;
    keyMapping.xValue = yValue;
    return keyMapping;
}

static KeyToTouchMappingInfo BuildKeyMapping(MappingTypeEnum mappingType)
{
    KeyToTouchMappingInfo keyMapping;
    keyMapping.mappingType = mappingType;
    switch (mappingType) {
        case MappingTypeEnum::DPAD_KEY_TO_TOUCH:
            keyMapping.dpadKeyCodeEntity.up = DPAD_UP;
            keyMapping.dpadKeyCodeEntity.down = DPAD_DOWN;
            keyMapping.dpadKeyCodeEntity.left = DPAD_LEFT;
            keyMapping.dpadKeyCodeEntity.right = DPAD_RIGHT;
            break;
        case MappingTypeEnum::KEY_BOARD_OBSERVATION_TO_TOUCH:
            keyMapping.dpadKeyCodeEntity.up = KEYBOARD_OBSERVATION_UP;
            keyMapping.dpadKeyCodeEntity.down = KEYBOARD_OBSERVATION_DOWN;
            keyMapping.dpadKeyCodeEntity.left = KEYBOARD_OBSERVATION_LEFT;
            keyMapping.dpadKeyCodeEntity.right = KEYBOARD_OBSERVATION_RIGHT;
            break;
        case MappingTypeEnum::SKILL_KEY_TO_TOUCH:
            keyMapping.skillRange = 1;
            keyMapping.radius = 1;
            break;
        default:
            break;
    }
    return keyMapping;
}

static GameKeyMappingInfo BuildCustomKeyMappingConfig()
{
    GameKeyMappingInfo keyMappingInfoConfig;
    keyMappingInfoConfig.bundleName = std::string(MAX_BUNDLE_NAME_LENGTH, 'a');
    keyMappingInfoConfig.deviceType = GAME_PAD;
    keyMappingInfoConfig.uniq = std::string(MAX_UNIQ_LENGTH, 'd');
    for (int32_t idx = 0; idx < MAX_KEY_MAPPING_SIZE; idx++) {
        keyMappingInfoConfig.customKeyToTouchMappings.push_back(BuildKeyMapping(idx, idx, idx));
    }
    return keyMappingInfoConfig;
}

static GameKeyMappingInfo BuildDefaultKeyMappingConfig()
{
    GameKeyMappingInfo keyMappingInfoConfig;
    keyMappingInfoConfig.bundleName = std::string(MAX_BUNDLE_NAME_LENGTH, 'a');
    keyMappingInfoConfig.deviceType = GAME_PAD;
    for (int32_t idx = 0; idx < MAX_KEY_MAPPING_SIZE; idx++) {
        keyMappingInfoConfig.defaultKeyToTouchMappings.push_back(BuildKeyMapping(idx, idx, idx));
    }
    return keyMappingInfoConfig;
}

static GameInfo BuildGameInfo()
{
    GameInfo gameInfo;
    gameInfo.bundleName = std::string(MAX_BUNDLE_NAME_LENGTH, 'a');
    gameInfo.version = std::string(MAX_VERSION_LENGTH, 'b');
    return gameInfo;
}

static GetGameKeyMappingInfoParam BuildGetGameKeyMappingInfoParam()
{
    GetGameKeyMappingInfoParam param;
    param.bundleName = std::string(MAX_BUNDLE_NAME_LENGTH, 'a');
    param.deviceType = GAME_PAD;
    param.uniq = std::string(MAX_UNIQ_LENGTH, 'c');
    return param;
}

static void CheckKeyMappingNumbers(GameKeyMappingInfo &config, MappingTypeEnum mappingType, int32_t number)
{
    std::vector<KeyToTouchMappingInfo> testKeyMappingInfos(number,
                                                           BuildKeyMapping(mappingType));
    for (int32_t i = 0; i < static_cast<int32_t>(testKeyMappingInfos.size()); ++i) {
        if (mappingType == MappingTypeEnum::COMBINATION_KEY_TO_TOUCH) {
            testKeyMappingInfos[i].combinationKeys = std::vector<int32_t>{i, i + MAX_COMBINATION_KEY_MAPPING_SIZE};
            continue;
        }
        testKeyMappingInfos[i].keyCode = i;
    }
    ASSERT_TRUE(config.CheckKeyMapping(testKeyMappingInfos));
    testKeyMappingInfos.push_back(BuildKeyMapping(mappingType));
    ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
}

/**
* @tc.name: GetGameKeyMappingInfoParam_CheckParamValid_001
* @tc.desc: GetGameKeyMappingInfoParam_CheckParamValid_001
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerKeymappingModeTest, GetGameKeyMappingInfoParam_CheckParamValid_001, TestSize.Level0)
{
    GetGameKeyMappingInfoParam param = BuildGetGameKeyMappingInfoParam();
    ASSERT_TRUE(param.CheckParamValid());

    param.bundleName = "";
    ASSERT_FALSE(param.CheckParamValid());
    param.bundleName = std::string(MAX_BUNDLE_NAME_LENGTH + 1, 'a');
    ASSERT_FALSE(param.CheckParamValid());
}

/**
* @tc.name: GameInfo_CheckParamValid_001
* @tc.desc: GameInfo_CheckParamValid_001
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerKeymappingModeTest, GameInfo_CheckParamValid_001, TestSize.Level0)
{
    GameInfo gameInfo = BuildGameInfo();
    ASSERT_TRUE(gameInfo.CheckParamValid());

    gameInfo.bundleName = "";
    ASSERT_FALSE(gameInfo.CheckParamValid());

    gameInfo.bundleName = std::string(MAX_BUNDLE_NAME_LENGTH + 1, 'a');
    ASSERT_FALSE(gameInfo.CheckParamValid());

    gameInfo = BuildGameInfo();
    gameInfo.version = "";
    ASSERT_FALSE(gameInfo.CheckParamValid());

    gameInfo.version = std::string(MAX_VERSION_LENGTH + 1, 'b');
    ASSERT_FALSE(gameInfo.CheckParamValid());
}

/**
* @tc.name: GameKeyMappingInfo_CheckParamValidForSetDefault_001
* @tc.desc: GameKeyMappingInfo_CheckParamValidForSetDefault_001
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerKeymappingModeTest, GameKeyMappingInfo_CheckParamValidForSetDefault_001, TestSize.Level0)
{
    // check bundleName
    GameKeyMappingInfo config = BuildDefaultKeyMappingConfig();

    config.bundleName = "";
    ASSERT_FALSE(config.CheckParamValidForSetDefault());
    config.bundleName = std::string(MAX_BUNDLE_NAME_LENGTH + 1, 'a');
    ASSERT_FALSE(config.CheckParamValidForSetDefault());

    // check defaultKeyToTouchMappings
    config = BuildDefaultKeyMappingConfig();
    config.defaultKeyToTouchMappings.push_back(BuildKeyMapping(-1, -1, -1));
    ASSERT_FALSE(config.CheckParamValidForSetDefault());
    config.defaultKeyToTouchMappings.clear();
    ASSERT_TRUE(config.CheckParamValidForSetDefault());
}

/**
* @tc.name: GameKeyMappingInfo_CheckParamValidForSetCustom_001
* @tc.desc: GameKeyMappingInfo_CheckParamValidForSetCustom_001
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerKeymappingModeTest, GameKeyMappingInfo_CheckParamValidForSetCustom_001, TestSize.Level0)
{
    // check bundleName
    GameKeyMappingInfo config = BuildCustomKeyMappingConfig();

    config.bundleName = "";
    ASSERT_FALSE(config.CheckParamValidForSetCustom());
    config.bundleName = std::string(MAX_BUNDLE_NAME_LENGTH + 1, 'a');
    ASSERT_FALSE(config.CheckParamValidForSetCustom());

    // check customKeyToTouchMappings
    config = BuildCustomKeyMappingConfig();
    config.customKeyToTouchMappings.push_back(BuildKeyMapping(-1, -1, -1));
    ASSERT_FALSE(config.CheckParamValidForSetCustom());
    config.customKeyToTouchMappings.clear();
    ASSERT_TRUE(config.CheckParamValidForSetCustom());
}

/**
* @tc.name: GameKeyMappingInfo_IsDelByBundleNameWhenSetDefault_001
* @tc.desc: GameKeyMappingInfo_IsDelByBundleNameWhenSetDefault_001
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerKeymappingModeTest, GameKeyMappingInfo_IsDelByBundleNameWhenSetDefault_001, TestSize.Level0)
{
    GameKeyMappingInfo config = BuildDefaultKeyMappingConfig();
    config.isOprDelete = true;
    ASSERT_FALSE(config.IsDelByBundleNameWhenSetDefault());

    config.isOprDelete = true;
    config.deviceType = UNKNOWN;
    ASSERT_TRUE(config.IsDelByBundleNameWhenSetDefault());
}

/**
* @tc.name: GameKeyMappingInfo_IsDelByBundleNameWhenSetCustom_001
* @tc.desc: GameKeyMappingInfo_IsDelByBundleNameWhenSetCustom_001
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerKeymappingModeTest, GameKeyMappingInfo_IsDelByBundleNameWhenSetCustom_001, TestSize.Level0)
{
    GameKeyMappingInfo config = BuildCustomKeyMappingConfig();
    config.isOprDelete = true;
    config.deviceType = GAME_PAD;
    ASSERT_FALSE(config.IsDelByBundleNameWhenSetCustom());

    config.deviceType = UNKNOWN;
    ASSERT_TRUE(config.IsDelByBundleNameWhenSetCustom());
}

/**
* @tc.name: GameKeyMappingInfo_CheckKeyMapping_001
* @tc.desc: Check the number of KeyToTouchMappingInfo for each mappingType
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerKeymappingModeTest, GameKeyMappingInfo_CheckKeyMapping_001, TestSize.Level0)
{
    GameKeyMappingInfo config = BuildDefaultKeyMappingConfig();

    CheckKeyMappingNumbers(config, MappingTypeEnum::SINGE_KEY_TO_TOUCH, MAX_SINGLE_KEY_MAPPING_SIZE);
    CheckKeyMappingNumbers(config, MappingTypeEnum::COMBINATION_KEY_TO_TOUCH, MAX_COMBINATION_KEY_MAPPING_SIZE);
    CheckKeyMappingNumbers(config, MappingTypeEnum::DPAD_KEY_TO_TOUCH, 1);
    CheckKeyMappingNumbers(config, MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH, 1);
    CheckKeyMappingNumbers(config, MappingTypeEnum::SKILL_KEY_TO_TOUCH, MAX_SKILL_KEY_MAPPING_SIZE);
    CheckKeyMappingNumbers(config, MappingTypeEnum::OBSERVATION_KEY_TO_TOUCH, 1);
    CheckKeyMappingNumbers(config, MappingTypeEnum::MOUSE_OBSERVATION_TO_TOUCH, 1);
    CheckKeyMappingNumbers(config, MappingTypeEnum::KEY_BOARD_OBSERVATION_TO_TOUCH, 1);
    CheckKeyMappingNumbers(config, MappingTypeEnum::CROSSHAIR_KEY_TO_TOUCH, 1);
    CheckKeyMappingNumbers(config, MappingTypeEnum::MOUSE_LEFT_FIRE_TO_TOUCH, 1);
    CheckKeyMappingNumbers(config, MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH, 1);
}

/**
* @tc.name: GameKeyMappingInfo_CheckKeyMapping_002
* @tc.desc: Check combination key size no more than 2
*           Check combination key cannot repeat
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerKeymappingModeTest, GameKeyMappingInfo_CheckKeyMapping_002, TestSize.Level0)
{
    GameKeyMappingInfo config = BuildDefaultKeyMappingConfig();
    std::vector<KeyToTouchMappingInfo> testKeyMappingInfos;
    KeyToTouchMappingInfo keyMapping = BuildKeyMapping(MappingTypeEnum::COMBINATION_KEY_TO_TOUCH);
    keyMapping.combinationKeys = {1, 2};
    testKeyMappingInfos.push_back(keyMapping);
    ASSERT_TRUE(config.CheckKeyMapping(testKeyMappingInfos));

    testKeyMappingInfos.clear();
    keyMapping.combinationKeys = {1, 1};
    testKeyMappingInfos.push_back(keyMapping);
    ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));

    testKeyMappingInfos.clear();
    keyMapping.combinationKeys = {1, 2, 3};
    testKeyMappingInfos.push_back(keyMapping);
    ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));

    testKeyMappingInfos.clear();
    keyMapping.combinationKeys = {1, 2};
    testKeyMappingInfos.push_back(keyMapping);
    keyMapping.combinationKeys = {3, 4};
    testKeyMappingInfos.push_back(keyMapping);
    ASSERT_TRUE(config.CheckKeyMapping(testKeyMappingInfos));

    keyMapping.combinationKeys = {2, 1};
    testKeyMappingInfos.push_back(keyMapping);
    ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
    testKeyMappingInfos.pop_back();
    keyMapping.combinationKeys = {1, 2};
    testKeyMappingInfos.push_back(keyMapping);
    ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
}

/**
* @tc.name: GameKeyMappingInfo_CheckKeyMapping_003
* @tc.desc: Check skill-key-mapping's skillRange and radius greater than 0
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerKeymappingModeTest, GameKeyMappingInfo_CheckKeyMapping_003, TestSize.Level0)
{
    GameKeyMappingInfo config = BuildDefaultKeyMappingConfig();
    std::vector<KeyToTouchMappingInfo> testKeyMappingInfos;
    KeyToTouchMappingInfo keyMapping = BuildKeyMapping(MappingTypeEnum::SKILL_KEY_TO_TOUCH);
    testKeyMappingInfos.push_back(keyMapping);
    ASSERT_TRUE(config.CheckKeyMapping(testKeyMappingInfos));

    testKeyMappingInfos.clear();
    keyMapping = BuildKeyMapping(MappingTypeEnum::SKILL_KEY_TO_TOUCH);
    keyMapping.skillRange = 0;
    keyMapping.radius = 1;
    testKeyMappingInfos.push_back(keyMapping);
    ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));

    testKeyMappingInfos.clear();
    keyMapping = BuildKeyMapping(MappingTypeEnum::SKILL_KEY_TO_TOUCH);
    keyMapping.skillRange = 1;
    keyMapping.radius = 0;
    testKeyMappingInfos.push_back(keyMapping);
    ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
}

/**
* @tc.name: GameKeyMappingInfo_CheckKeyMapping_004
* @tc.desc: Keycode for mapping type 0、2、4、5、7、8 cannot repeat
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerKeymappingModeTest, GameKeyMappingInfo_CheckKeyMapping_004, TestSize.Level0)
{
    GameKeyMappingInfo config = BuildDefaultKeyMappingConfig();
    std::vector<KeyToTouchMappingInfo> testKeyMappingInfos;
    testKeyMappingInfos.push_back(BuildKeyMapping(MappingTypeEnum::SINGE_KEY_TO_TOUCH));
    testKeyMappingInfos.push_back(BuildKeyMapping(MappingTypeEnum::DPAD_KEY_TO_TOUCH));
    testKeyMappingInfos.push_back(BuildKeyMapping(MappingTypeEnum::SKILL_KEY_TO_TOUCH));
    testKeyMappingInfos.push_back(BuildKeyMapping(MappingTypeEnum::OBSERVATION_KEY_TO_TOUCH));
    testKeyMappingInfos.push_back(BuildKeyMapping(MappingTypeEnum::KEY_BOARD_OBSERVATION_TO_TOUCH));
    testKeyMappingInfos.push_back(BuildKeyMapping(MappingTypeEnum::CROSSHAIR_KEY_TO_TOUCH));
    for (int i = 0; i < testKeyMappingInfos.size(); ++i) {
        MappingTypeEnum type = testKeyMappingInfos[i].mappingType;
        if (type == MappingTypeEnum::DPAD_KEY_TO_TOUCH || type == MappingTypeEnum::KEY_BOARD_OBSERVATION_TO_TOUCH) {
            continue;
        }
        testKeyMappingInfos[i].keyCode = i;
    }
    ASSERT_TRUE(config.CheckKeyMapping(testKeyMappingInfos));

    for (int i = 1; i < testKeyMappingInfos.size(); ++i) {
        if (testKeyMappingInfos[i].mappingType == MappingTypeEnum::DPAD_KEY_TO_TOUCH) {
            testKeyMappingInfos[i].dpadKeyCodeEntity.up = 0;
            ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
            testKeyMappingInfos[i].dpadKeyCodeEntity.up = DPAD_UP;
            testKeyMappingInfos[i].dpadKeyCodeEntity.down = 0;
            ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
            testKeyMappingInfos[i].dpadKeyCodeEntity.down = DPAD_DOWN;
            testKeyMappingInfos[i].dpadKeyCodeEntity.left = 0;
            ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
            testKeyMappingInfos[i].dpadKeyCodeEntity.left = DPAD_LEFT;
            testKeyMappingInfos[i].dpadKeyCodeEntity.right = 0;
            ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
            testKeyMappingInfos[i].dpadKeyCodeEntity.right = DPAD_RIGHT;
        } else if (testKeyMappingInfos[i].mappingType == MappingTypeEnum::KEY_BOARD_OBSERVATION_TO_TOUCH) {
            testKeyMappingInfos[i].dpadKeyCodeEntity.up = 0;
            ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
            testKeyMappingInfos[i].dpadKeyCodeEntity.up = KEYBOARD_OBSERVATION_UP;
            testKeyMappingInfos[i].dpadKeyCodeEntity.down = 0;
            ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
            testKeyMappingInfos[i].dpadKeyCodeEntity.down = KEYBOARD_OBSERVATION_DOWN;
            testKeyMappingInfos[i].dpadKeyCodeEntity.left = 0;
            ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
            testKeyMappingInfos[i].dpadKeyCodeEntity.left = KEYBOARD_OBSERVATION_LEFT;
            testKeyMappingInfos[i].dpadKeyCodeEntity.right = 0;
            ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
            testKeyMappingInfos[i].dpadKeyCodeEntity.right = KEYBOARD_OBSERVATION_RIGHT;
        } else {
            testKeyMappingInfos[i].keyCode = 0;
            ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
            testKeyMappingInfos[i].keyCode = i;
        }
    }
}

/**
* @tc.name: GameKeyMappingInfo_CheckKeyMapping_005
* @tc.desc: Mapping type 3 and 6 and 10 cannot exit simultaneously
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerKeymappingModeTest, GameKeyMappingInfo_CheckKeyMapping_005, TestSize.Level0)
{
    GameKeyMappingInfo config = BuildDefaultKeyMappingConfig();
    std::vector<MappingTypeEnum> mouseRightVector;
    mouseRightVector.push_back(MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH);
    mouseRightVector.push_back(MappingTypeEnum::MOUSE_OBSERVATION_TO_TOUCH);
    mouseRightVector.push_back(MappingTypeEnum::MOUSE_RIGHT_KEY_CLICK_TO_TOUCH);
    std::vector<KeyToTouchMappingInfo> testKeyMappingInfos;
    for (const auto &mouseRight: mouseRightVector) {
        for (const auto &other: mouseRightVector) {
            testKeyMappingInfos.clear();
            testKeyMappingInfos.push_back(BuildKeyMapping(mouseRight));
            if (other == mouseRight) {
                ASSERT_TRUE(config.CheckKeyMapping(testKeyMappingInfos));
                continue;
            }
            testKeyMappingInfos.push_back(BuildKeyMapping(other));
            ASSERT_FALSE(config.CheckKeyMapping(testKeyMappingInfos));
        }
    }
}

/**
* @tc.name: GameKeyMappingInfo_CheckKeyMappingForHoverTouchPad_001
* @tc.desc: HoverTouchPad only support 2 SINGE_KEY_TO_TOUCH key mappings
*           Keycode cannot repeat
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerKeymappingModeTest, GameKeyMappingInfo_CheckKeyMappingForHoverTouchPad_001, TestSize.Level0)
{
    GameKeyMappingInfo config = BuildDefaultKeyMappingConfig();
    config.deviceType = DeviceTypeEnum::HOVER_TOUCH_PAD;
    std::vector<KeyToTouchMappingInfo> testKeyMappingInfos;
    KeyToTouchMappingInfo keyMapping = BuildKeyMapping(MappingTypeEnum::SINGE_KEY_TO_TOUCH);
    testKeyMappingInfos.push_back(keyMapping);
    ASSERT_TRUE(config.CheckKeyMapping(testKeyMappingInfos));

    for (int i = 1; i < static_cast<int>(SUM_OF_MAPPING_TYPE); ++i) {
        testKeyMappingInfos.clear();
        keyMapping = BuildKeyMapping(static_cast<MappingTypeEnum>(i));
        testKeyMappingInfos.push_back(keyMapping);
        ASSERT_FALSE(config.CheckKeyMappingForHoverTouchPad(testKeyMappingInfos));
    }

    testKeyMappingInfos.clear();
    keyMapping = BuildKeyMapping(MappingTypeEnum::SINGE_KEY_TO_TOUCH);
    testKeyMappingInfos.push_back(keyMapping);
    keyMapping = BuildKeyMapping(MappingTypeEnum::SINGE_KEY_TO_TOUCH);
    keyMapping.keyCode = 1;
    testKeyMappingInfos.push_back(keyMapping);
    ASSERT_TRUE(config.CheckKeyMappingForHoverTouchPad(testKeyMappingInfos));

    keyMapping = BuildKeyMapping(MappingTypeEnum::SINGE_KEY_TO_TOUCH);
    keyMapping.keyCode = 2;
    testKeyMappingInfos.push_back(keyMapping);
    ASSERT_FALSE(config.CheckKeyMappingForHoverTouchPad(testKeyMappingInfos));

    testKeyMappingInfos.pop_back();
    testKeyMappingInfos.back().keyCode = 0;
    ASSERT_FALSE(config.CheckKeyMappingForHoverTouchPad(testKeyMappingInfos));
}
}
}