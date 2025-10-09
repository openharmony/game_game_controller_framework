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

#include "game_support_key_mapping_manager.h"
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
const size_t MAX_CONFIG_NUM = 2000;
}
class GameSupportKeyMappingManagerTest : public testing::Test {
public:
    void SetUp();

    void TearDown();

    void ClearCache();
};

static GameInfo BuildGameInfo(const std::string &bundleName, const std::string &version, const bool isSupportKeyMapping)
{
    GameInfo gameInfo;
    gameInfo.bundleName = bundleName;
    gameInfo.version = version;
    gameInfo.isSupportKeyMapping = isSupportKeyMapping;
    return gameInfo;
}

static GameSupportKeyMappingConfig BuildGameConfig(const std::string &bundleName, const std::string &version)
{
    GameSupportKeyMappingConfig gameConfig;
    gameConfig.bundleName = bundleName;
    gameConfig.version = version;
    return gameConfig;
}

void GameSupportKeyMappingManagerTest::SetUp()
{
    ClearCache();
}

void GameSupportKeyMappingManagerTest::TearDown()
{
    ClearCache();
}

void GameSupportKeyMappingManagerTest::ClearCache()
{
    DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->configMap_.clear();
}

/**
 * @tc.name: SyncSupportKeyMappingGames_001
 * @tc.desc: check param valid.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(GameSupportKeyMappingManagerTest, SyncSupportKeyMappingGames_001, TestSize.Level1)
{
    // 1. check bundleName is empty
    std::vector<GameInfo> gameInfos;
    GameInfo gameInfo = BuildGameInfo("", "121", true);
    gameInfos.push_back(gameInfo);
    int32_t rtn = DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->SyncSupportKeyMappingGames(false,
                                                                                                            gameInfos);
    ASSERT_EQ(GAME_ERR_ARGUMENT_INVALID, rtn);

    //gameInfos is over max number
    gameInfos.clear();
    for (int32_t idx = 0; idx <= MAX_CONFIG_NUM; idx++) {
        gameInfos.push_back(BuildGameInfo(std::to_string(idx), "121", true));
    }
    rtn = DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->SyncSupportKeyMappingGames(false,
                                                                                                    gameInfos);
    ASSERT_EQ(GAME_ERR_ARGUMENT_INVALID, rtn);


    // gameInfos is not over max number
    gameInfos.clear();
    for (int32_t idx = 0; idx < MAX_CONFIG_NUM; idx++) {
        gameInfos.push_back(BuildGameInfo(std::to_string(idx), "121", true));
    }
    rtn = DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->SyncSupportKeyMappingGames(false,
                                                                                                    gameInfos);
    ASSERT_EQ(GAME_CONTROLLER_SUCCESS, rtn);

    // the number of support key-mapping games is over max number
    gameInfos.clear();
    gameInfos.push_back(BuildGameInfo("deviceId", "121", true));
    rtn = DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->SyncSupportKeyMappingGames(false,
                                                                                                gameInfos);
    ASSERT_EQ(GAME_ERR_ARGUMENT_INVALID, rtn);
}

static void CheckGameConfig(GameInfo &gameInfo, GameSupportKeyMappingConfig &gameConfig)
{
    ASSERT_EQ(gameInfo.bundleName, gameConfig.bundleName);
    ASSERT_EQ(gameInfo.version, gameConfig.version);
}

/**
 * @tc.name: SyncSupportKeyMappingGames_002
 * @tc.desc: When all configurations are synchronized, the original configurations are deleted.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(GameSupportKeyMappingManagerTest, SyncSupportKeyMappingGames_002, TestSize.Level0)
{
    GameSupportKeyMappingConfig gameConfig = BuildGameConfig("0", "00");
    DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->configMap_[gameConfig.bundleName] = gameConfig;
    std::vector<GameInfo> gameInfos;
    GameInfo gameInfo1 = BuildGameInfo("1", "11", true);
    gameInfos.push_back(gameInfo1);
    GameInfo gameInfo2 = BuildGameInfo("2", "22", true);
    gameInfos.push_back(gameInfo2);
    GameInfo gameInfo3 = BuildGameInfo("3", "33", false);
    gameInfos.push_back(gameInfo3);

    // execute
    int32_t rtn = DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->SyncSupportKeyMappingGames(true,
                                                                                                            gameInfos);

    // check data
    ASSERT_EQ(GAME_CONTROLLER_SUCCESS, rtn);
    ASSERT_EQ(2, DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->configMap_.size());
    CheckGameConfig(gameInfo1, DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()
        ->configMap_[gameInfo1.bundleName]);
    CheckGameConfig(gameInfo2, DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()
        ->configMap_[gameInfo2.bundleName]);
}

/**
 * @tc.name: SyncSupportKeyMappingGames_003
 * @tc.desc: When not all Configurations are synchronized, modify configurations.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(GameSupportKeyMappingManagerTest, SyncSupportKeyMappingGames_003, TestSize.Level0)
{
    GameSupportKeyMappingConfig gameConfig1 = BuildGameConfig("0", "00");
    DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->configMap_[gameConfig1.bundleName] = gameConfig1;
    GameSupportKeyMappingConfig gameConfig2 = BuildGameConfig("2", "22");
    DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->configMap_[gameConfig2.bundleName] = gameConfig2;
    std::vector<GameInfo> gameInfos;
    GameInfo gameInfo1 = BuildGameInfo("0", "11", true);
    gameInfos.push_back(gameInfo1);
    GameInfo gameInfo2 = BuildGameInfo("2", "22", false);
    gameInfos.push_back(gameInfo2);
    GameInfo gameInfo3 = BuildGameInfo("3", "33", true);
    gameInfos.push_back(gameInfo3);

    // execute
    int32_t rtn = DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->SyncSupportKeyMappingGames(false,
                                                                                                            gameInfos);

    // check data
    ASSERT_EQ(GAME_CONTROLLER_SUCCESS, rtn);
    ASSERT_EQ(2, DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->configMap_.size());
    CheckGameConfig(gameInfo1, DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()
        ->configMap_[gameInfo1.bundleName]);
    CheckGameConfig(gameInfo3, DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()
        ->configMap_[gameInfo3.bundleName]);
}

/**
 * @tc.name: IsSupportGameKeyMapping_001
 * @tc.desc: Game supports key mapping
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(GameSupportKeyMappingManagerTest, IsSupportGameKeyMapping_001, TestSize.Level0)
{
    GameSupportKeyMappingConfig gameConfig = BuildGameConfig("0", "00");
    DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->configMap_[gameConfig.bundleName] = gameConfig;
    GameInfo gameInfo = BuildGameInfo("0", "11", true);
    GameInfo result;

    //execute
    int32_t rtn = DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->IsSupportGameKeyMapping(gameInfo,
                                                                                                         result);
    // check data
    ASSERT_EQ(GAME_CONTROLLER_SUCCESS, rtn);
    ASSERT_TRUE(result.isSupportKeyMapping);
    ASSERT_EQ(gameConfig.version, result.version);
}

/**
 * @tc.name: IsSupportGameKeyMapping_002
 * @tc.desc: Game does not support key mapping.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(GameSupportKeyMappingManagerTest, IsSupportGameKeyMapping_002, TestSize.Level1)
{
    GameInfo gameInfo = BuildGameInfo("22", "11", true);
    GameInfo result;

    // execute
    int32_t rtn = DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->IsSupportGameKeyMapping(gameInfo,
                                                                                                         result);

    // check data
    ASSERT_EQ(GAME_CONTROLLER_SUCCESS, rtn);
    ASSERT_FALSE(result.isSupportKeyMapping);
}

/**
 * @tc.name: LoadConfigFromJsonFile_001
 * @tc.desc: load config success
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(GameSupportKeyMappingManagerTest, LoadConfigFromJsonFile_001, TestSize.Level0)
{
    std::vector<GameInfo> gameInfos;
    GameInfo gameInfo1 = BuildGameInfo("1", "11", true);
    gameInfos.push_back(gameInfo1);
    GameInfo gameInfo2 = BuildGameInfo("2", "22", true);
    gameInfos.push_back(gameInfo2);
    DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->SyncSupportKeyMappingGames(true, gameInfos);
    DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->configMap_.clear();

    // execute
    DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->LoadConfigFromJsonFile();

    // check data
    ASSERT_EQ(2, DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()->configMap_.size());
    CheckGameConfig(gameInfo1, DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()
        ->configMap_[gameInfo1.bundleName]);
    CheckGameConfig(gameInfo2, DelayedSingleton<GameSupportKeyMappingManager>::GetInstance()
        ->configMap_[gameInfo2.bundleName]);
}
}
}