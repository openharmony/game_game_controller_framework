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
#include "gamecontroller_keymapping_model.h"
#include <gtest/gtest.h>
#include "refbase.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
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
    ASSERT_TRUE(config.CheckParamValidForSetDefault());
    config.bundleName = "";
    ASSERT_FALSE(config.CheckParamValidForSetDefault());
    config.bundleName = std::string(MAX_BUNDLE_NAME_LENGTH + 1, 'a');
    ASSERT_FALSE(config.CheckParamValidForSetDefault());

    // check defaultKeyToTouchMappings
    config = BuildDefaultKeyMappingConfig();
    config.defaultKeyToTouchMappings.push_back(BuildKeyMapping(-1, -1, -1));
    ASSERT_FALSE(config.CheckParamValidForSetDefault());
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
    ASSERT_TRUE(config.CheckParamValidForSetCustom());
    config.bundleName = "";
    ASSERT_FALSE(config.CheckParamValidForSetCustom());
    config.bundleName = std::string(MAX_BUNDLE_NAME_LENGTH + 1, 'a');
    ASSERT_FALSE(config.CheckParamValidForSetCustom());

    // check customKeyToTouchMappings
    config = BuildCustomKeyMappingConfig();
    config.customKeyToTouchMappings.push_back(BuildKeyMapping(-1, -1, -1));
    ASSERT_FALSE(config.CheckParamValidForSetCustom());
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
}
}