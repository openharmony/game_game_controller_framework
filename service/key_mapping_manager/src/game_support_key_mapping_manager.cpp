/*
 *  Copyright (c) 2025 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <gamecontroller_log.h>
#include <gamecontroller_errors.h>
#include "game_support_key_mapping_manager.h"
#include "json_utils.h"

using json = nlohmann::json;
namespace OHOS {
namespace GameController {
namespace {
const std::string BUNDLE_SUPPORT_KEYMAPPING_CFG = "/data/service/el1/public/for-all-app/gamecontroller_server/";
constexpr const char* FIELD_BUNDLE_NAME = "bundleName";
constexpr const char* FIELD_VERSION = "version";
constexpr const char* CONFIG_FILE = "game_support_key_mapping.json";
const size_t MAX_CONFIG_NUM = 2000;
}

GameSupportKeyMappingConfig::GameSupportKeyMappingConfig(const json &jsonObj)
{
    if (jsonObj.contains(FIELD_BUNDLE_NAME) && jsonObj.at(FIELD_BUNDLE_NAME).is_string()) {
        this->bundleName = jsonObj.at(FIELD_BUNDLE_NAME).get<std::string>();
    }
    if (jsonObj.contains(FIELD_VERSION) && jsonObj.at(FIELD_VERSION).is_string()) {
        this->version = jsonObj.at(FIELD_VERSION).get<std::string>();
    }
}

json GameSupportKeyMappingConfig::ConvertToJson() const
{
    json jsonContent;
    jsonContent[FIELD_BUNDLE_NAME] = this->bundleName;
    jsonContent[FIELD_VERSION] = this->version;
    return jsonContent;
}

GameSupportKeyMappingManager::~GameSupportKeyMappingManager()
{
}

GameSupportKeyMappingManager::GameSupportKeyMappingManager()
{
    LoadConfigFromJsonFile();
}

int32_t GameSupportKeyMappingManager::IsSupportGameKeyMapping(const GameInfo &gameInfo, GameInfo &resultGameInfo)
{
    std::lock_guard<std::mutex> lock(mutex_);
    resultGameInfo.bundleName = gameInfo.bundleName;
    if (configMap_.find(gameInfo.bundleName) != configMap_.end()) {
        GameSupportKeyMappingConfig gameConfig = configMap_[gameInfo.bundleName];
        resultGameInfo.version = gameConfig.version;
        resultGameInfo.isSupportKeyMapping = true;
        HILOGI("[%{public}s] supports key-mapping", gameInfo.bundleName.c_str());
    } else {
        resultGameInfo.isSupportKeyMapping = false;
        HILOGI("[%{public}s] dose not support key-mapping", gameInfo.bundleName.c_str());
    }
    return GAME_CONTROLLER_SUCCESS;
}

int32_t GameSupportKeyMappingManager::SyncSupportKeyMappingGames(bool isSyncAll,
                                                                 const std::vector<GameInfo> &gameInfos)
{
    if (gameInfos.size() > MAX_CONFIG_NUM) {
        HILOGE("It's over max config for SyncSupportKeyMappingGames");
        return GAME_ERR_ARGUMENT_INVALID;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    std::unordered_map<std::string, GameSupportKeyMappingConfig> tempMap;
    std::vector<std::string> delBundles;
    if (!isSyncAll && !configMap_.empty()) {
        tempMap.insert(configMap_.begin(), configMap_.end());
    }
    for (const auto &gameInfo: gameInfos) {
        if (!gameInfo.CheckParamValid() || !JsonUtils::IsUtf8(gameInfo.bundleName) ||
            !JsonUtils::IsUtf8(gameInfo.version)) {
            HILOGE("CheckParamValid For SyncSupportKeyMappingGames failed");
            return GAME_ERR_ARGUMENT_INVALID;
        }
        if (gameInfo.isSupportKeyMapping) {
            tempMap[gameInfo.bundleName] = GameSupportKeyMappingConfig(gameInfo);
        } else {
            delBundles.push_back(gameInfo.bundleName);
        }
    }

    // if not support keymapping, need to delete game config
    for (const auto &delBundle: delBundles) {
        if (tempMap.find(delBundle) != tempMap.end()) {
            tempMap.erase(delBundle);
        }
    }

    if (tempMap.size() > MAX_CONFIG_NUM) {
        HILOGE("It's over max config for SyncSupportKeyMappingGames");
        return GAME_ERR_ARGUMENT_INVALID;
    }

    if (SaveToConfigFile(tempMap)) {
        HILOGI("save GameSupportKeyMappingConfig success");
        configMap_.clear();
        configMap_.insert(tempMap.begin(), tempMap.end());
        return GAME_CONTROLLER_SUCCESS;
    }

    HILOGE("save GameSupportKeyMappingConfig failed");
    return GAME_ERR_FAIL;
}

void GameSupportKeyMappingManager::LoadConfigFromJsonFile()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!JsonUtils::IsFileExist(BUNDLE_SUPPORT_KEYMAPPING_CFG + CONFIG_FILE)) {
        HILOGI("[%{public}s] does not exist in service directory.", CONFIG_FILE);
        if (!JsonUtils::CopyFile(GAME_CONTROLLER_ETC_ROOT + CONFIG_FILE,
                                 BUNDLE_SUPPORT_KEYMAPPING_CFG + CONFIG_FILE,
                                 true)) {
            HILOGE("[%{public}s] copy to service directory failed.", CONFIG_FILE);
            return;
        }
    }

    configMap_.clear();
    std::pair<bool, json> ret = JsonUtils::ReadJsonFromFile(BUNDLE_SUPPORT_KEYMAPPING_CFG + CONFIG_FILE);
    if (!ret.first) {
        HILOGW("load [%{public}s] file failed.", CONFIG_FILE);
        return;
    }
    HILOGI("load [%{public}s] file success.", CONFIG_FILE);
    json config = ret.second;
    if (config.empty()) {
        HILOGW("[%{public}s] no config.", CONFIG_FILE);
        return;
    }
    if (!config.is_array()) {
        HILOGW("[%{public}s] is not json array.", CONFIG_FILE);
        return;
    }
    for (const auto &jsonObj: config) {
        GameSupportKeyMappingConfig gameConfig(jsonObj);
        configMap_.insert(make_pair(gameConfig.bundleName, gameConfig));
        HILOGI("game [%{public}s] is support .", gameConfig.bundleName.c_str());
    }
    HILOGI("load [%{public}s] file success. size is [%{public}d]", CONFIG_FILE, configMap_.size());
}

bool GameSupportKeyMappingManager::SaveToConfigFile(const std::unordered_map<std::string,
    GameSupportKeyMappingConfig> &configMap)
{
    json jsonContent = json::array();
    for (const auto &config: configMap) {
        jsonContent.push_back(config.second.ConvertToJson());
    }
    return JsonUtils::WriteFileFromJson(BUNDLE_SUPPORT_KEYMAPPING_CFG + CONFIG_FILE, jsonContent);
}
}
}