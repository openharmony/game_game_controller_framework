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

#include <string>
#include <vector>
#include <gamecontroller_errors.h>
#include "key_mapping_config_manager.h"
#include "gamecontroller_log.h"
#include "json_utils.h"
#include "event_publisher.h"

using json = nlohmann::json;

namespace OHOS {
namespace GameController {
namespace {
constexpr const char* FIELD_KEY_CODE = "keyCode";
constexpr const char* FIELD_KEY_MAPPING = "keyMapping";
constexpr const char* FIELD_MAPPING_TYPE = "mappingType";
constexpr const char* FIELD_DEVICE_TYPE = "deviceType";
constexpr const char* FIELD_X_VALUE = "xValue";
constexpr const char* FIELD_Y_VALUE = "yValue";
constexpr const char* FIELD_BUNDLE_NAME = "bundleName";
constexpr const char* FIELD_RADIUS = "radius";
constexpr const char* FIELD_SKILL_RANGE = "skillRange";
constexpr const char* FIELD_X_STEP = "xStep";
constexpr const char* FIELD_Y_STEP = "yStep";
constexpr const char* FIELD_COMBINATION_KEYS = "combinationKeys";
constexpr const char* FIELD_DPAD = "dpad";
constexpr const char* FIELD_UP = "up";
constexpr const char* FIELD_DOWN = "down";
constexpr const char* FIELD_LEFT = "left";
constexpr const char* FIELD_RIGHT = "right";
constexpr const char* FIELD_DELAY_TIME = "delayTime";
constexpr const char* DEFAULT_KEY_MAPPING_CONFIG = "default_key_mapping.json";

constexpr const char* CUSTOM_KEY_MAPPING_CONFIG = "custom_key_mapping.json";
}

DpadInfo::DpadInfo(const json &jsonObj)
{
    up = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_UP, 0);
    down = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_DOWN, 0);
    left = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_LEFT, 0);
    right = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_RIGHT, 0);
}

json DpadInfo::ConvertToJson() const
{
    json jsonContent;
    jsonContent[FIELD_UP] = this->up;
    jsonContent[FIELD_DOWN] = this->down;
    jsonContent[FIELD_LEFT] = this->left;
    jsonContent[FIELD_RIGHT] = this->right;
    return jsonContent;
}

KeyMapping::KeyMapping(const json &jsonObj)
{
    mappingType = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_MAPPING_TYPE, 0);
    keyCode = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_KEY_CODE, 0);
    xValue = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_X_VALUE, 0);
    yValue = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_Y_VALUE, 0);
    radius = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_RADIUS, 0);
    skillRange = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_SKILL_RANGE, 0);
    xStep = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_X_STEP, 0);
    yStep = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_Y_STEP, 0);
    delayTime = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_DELAY_TIME, 0);

    if (jsonObj.contains(FIELD_COMBINATION_KEYS) && jsonObj.at(FIELD_COMBINATION_KEYS).is_array()) {
        combinationKeys = jsonObj.at(FIELD_COMBINATION_KEYS).get<std::vector<int32_t>>();
    }

    if (jsonObj.contains(FIELD_DPAD) && jsonObj.at(FIELD_DPAD).is_object()) {
        dpadInfo = DpadInfo(jsonObj.at(FIELD_DPAD));
    }
}

json KeyMapping::ConvertToJson() const
{
    json jsonContent;
    jsonContent[FIELD_MAPPING_TYPE] = this->mappingType;
    jsonContent[FIELD_KEY_CODE] = this->keyCode;
    jsonContent[FIELD_X_VALUE] = this->xValue;
    jsonContent[FIELD_Y_VALUE] = this->yValue;
    jsonContent[FIELD_RADIUS] = this->radius;
    jsonContent[FIELD_SKILL_RANGE] = this->skillRange;
    jsonContent[FIELD_X_STEP] = this->xStep;
    jsonContent[FIELD_Y_STEP] = this->yStep;
    jsonContent[FIELD_DELAY_TIME] = this->delayTime;
    jsonContent[FIELD_DPAD] = this->dpadInfo.ConvertToJson();
    jsonContent[FIELD_COMBINATION_KEYS] = combinationKeys;
    return jsonContent;
}

KeyMappingInfoConfig::KeyMappingInfoConfig(const json &jsonObj, const bool isCustom)
{
    if (jsonObj.contains(FIELD_BUNDLE_NAME) && jsonObj.at(FIELD_BUNDLE_NAME).is_string()) {
        this->bundleName = jsonObj.at(FIELD_BUNDLE_NAME).get<std::string>();
    }

    this->deviceType = JsonUtils::GetJsonInt32Value(jsonObj, FIELD_DEVICE_TYPE, 0);

    if (jsonObj.contains(FIELD_DEVICE_TYPE) && jsonObj.at(FIELD_DEVICE_TYPE).is_number_integer()) {
        this->deviceType = jsonObj.at(FIELD_DEVICE_TYPE).get<int32_t>();
    }

    if (!jsonObj.contains(FIELD_KEY_MAPPING) || !jsonObj.at(FIELD_KEY_MAPPING).is_array()) {
        HILOGW("keyMapping is not json array.");
        return;
    }
    auto dataJson = jsonObj[FIELD_KEY_MAPPING];
    if (!dataJson.is_array()) {
        HILOGW("keyMapping is not json array.");
        return;
    }

    std::vector<KeyMapping> keyMappings;
    for (const auto &js: dataJson) {
        KeyMapping mapping(js);
        keyMappings.push_back(mapping);
    }

    if (isCustom) {
        this->customKeyMappings = keyMappings;
    } else {
        this->defaultKeyMappings = keyMappings;
    }
}

json KeyMappingInfoConfig::ConvertToJson(const bool isCustom) const
{
    json jsonContent;
    jsonContent[FIELD_BUNDLE_NAME] = this->bundleName;
    jsonContent[FIELD_DEVICE_TYPE] = this->deviceType;
    if (isCustom) {
        for (const auto &mapping: this->customKeyMappings) {
            jsonContent[FIELD_KEY_MAPPING].push_back(mapping.ConvertToJson());
        }
    } else {
        for (const auto &mapping: this->defaultKeyMappings) {
            jsonContent[FIELD_KEY_MAPPING].push_back(mapping.ConvertToJson());
        }
    }
    return jsonContent;
}

KeyMappingConfigManager::~KeyMappingConfigManager()
{
}

KeyMappingConfigManager::KeyMappingConfigManager()
{
    LoadConfigFromJsonFile();
}

bool KeyMappingConfigManager::SaveToConfigFile(const std::unordered_map<std::string, KeyMappingInfoConfig> &configMap,
                                               const std::string &filePath, const bool isCustom)
{
    json jsonContent = json::array();
    for (const auto &config: configMap) {
        jsonContent.push_back(config.second.ConvertToJson(isCustom));
    }
    return JsonUtils::WriteFileFromJson(filePath, jsonContent);
}

void KeyMappingConfigManager::LoadConfigFromJsonFile()
{
    std::lock_guard<std::mutex> lock(mutex_);
    bool isLoadConfig = true;
    if (!JsonUtils::IsFileExist(GAME_CONTROLLER_SERVICE_ROOT + DEFAULT_KEY_MAPPING_CONFIG)) {
        HILOGI("[%{public}s] does not exist in service directory.", DEFAULT_KEY_MAPPING_CONFIG);
        if (!JsonUtils::CopyFile(GAME_CONTROLLER_ETC_ROOT + DEFAULT_KEY_MAPPING_CONFIG,
                                 GAME_CONTROLLER_SERVICE_ROOT + DEFAULT_KEY_MAPPING_CONFIG,
                                 false)) {
            HILOGE("[%{public}s] copy to service directory failed.", DEFAULT_KEY_MAPPING_CONFIG);
            isLoadConfig = false;
        }
    }

    if (isLoadConfig) {
        std::unordered_map<std::string, KeyMappingInfoConfig> defaultConfigMap = LoadConfigFromJsonFile(
            GAME_CONTROLLER_SERVICE_ROOT + DEFAULT_KEY_MAPPING_CONFIG, false);
        defaultKeyMappingInfoConfigMap_.clear();
        defaultKeyMappingInfoConfigMap_.insert(defaultConfigMap.begin(), defaultConfigMap.end());
        HILOGI("default_key_mapping size is [%{public}d].", static_cast<int>(defaultConfigMap.size()));
    }

    isLoadConfig = true;
    if (!JsonUtils::IsFileExist(GAME_CONTROLLER_SERVICE_ROOT + CUSTOM_KEY_MAPPING_CONFIG)) {
        HILOGI("[%{public}s] does not exist in service directory.", CUSTOM_KEY_MAPPING_CONFIG);
        if (!JsonUtils::CopyFile(GAME_CONTROLLER_ETC_ROOT + CUSTOM_KEY_MAPPING_CONFIG,
                                 GAME_CONTROLLER_SERVICE_ROOT + CUSTOM_KEY_MAPPING_CONFIG,
                                 false)) {
            HILOGE("[%{public}s] copy to service directory failed.", CUSTOM_KEY_MAPPING_CONFIG);
            isLoadConfig = false;
        }
    }
    if (isLoadConfig) {
        std::unordered_map<std::string, KeyMappingInfoConfig> customConfigMap = LoadConfigFromJsonFile(
            GAME_CONTROLLER_SERVICE_ROOT + CUSTOM_KEY_MAPPING_CONFIG, true);
        customKeyMappingInfoConfigMap_.clear();
        customKeyMappingInfoConfigMap_.insert(customConfigMap.begin(), customConfigMap.end());
        HILOGI("custom_key_mapping size is [%{public}d].", static_cast<int>(customConfigMap.size()));
    }
}

std::unordered_map<std::string, KeyMappingInfoConfig> KeyMappingConfigManager::LoadConfigFromJsonFile(
    const std::string &filePath, const bool isCustom)
{
    std::unordered_map<std::string, KeyMappingInfoConfig> configMap;
    std::pair<bool, json> ret = JsonUtils::ReadJsonFromFile(filePath);
    if (!ret.first) {
        HILOGW("load [%{public}s] file failed.", filePath.c_str());
        return configMap;
    }
    HILOGI("load [%{public}s] file success.", filePath.c_str());
    json config = ret.second;
    if (config.empty()) {
        HILOGW("[%{public}s] no config.", filePath.c_str());
        return configMap;
    }
    if (!config.is_array()) {
        HILOGW("[%{public}s]'s not json.", filePath.c_str());
        return configMap;
    }
    for (const auto &jsonObj: config) {
        KeyMappingInfoConfig keyMappingInfoConfig(jsonObj, isCustom);
        if (isCustom) {
            configMap.insert(make_pair(keyMappingInfoConfig.GetKeyForCustomKeyMapping(), keyMappingInfoConfig));
        } else {
            configMap.insert(make_pair(keyMappingInfoConfig.GetKeyForDefaultKeyMapping(), keyMappingInfoConfig));
        }
    }
    return configMap;
}

int32_t KeyMappingConfigManager::SetDefaultGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfoForConst)
{
    GameKeyMappingInfo gameKeyMappingInfo = gameKeyMappingInfoForConst;
    if (!gameKeyMappingInfo.CheckParamValidForSetDefault() || !JsonUtils::IsUtf8(gameKeyMappingInfo.bundleName)) {
        HILOGE("CheckParamValidForSetDefault failed");
        return GAME_ERR_ARGUMENT_INVALID;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    std::unordered_map<std::string, KeyMappingInfoConfig> configMap;
    configMap.insert(defaultKeyMappingInfoConfigMap_.begin(), defaultKeyMappingInfoConfigMap_.end());
    KeyMappingInfoConfig config(gameKeyMappingInfo);
    std::string key = config.GetKeyForDefaultKeyMapping();
    bool isDelByBundleName = gameKeyMappingInfo.IsDelByBundleNameWhenSetDefault();
    if (gameKeyMappingInfo.isOprDelete) {
        DelFromMap(configMap, key, isDelByBundleName);
    } else {
        configMap[key] = config;
    }
    if (SaveToConfigFile(configMap, GAME_CONTROLLER_SERVICE_ROOT + DEFAULT_KEY_MAPPING_CONFIG, false)) {
        if (gameKeyMappingInfo.isOprDelete) {
            DelFromMap(defaultKeyMappingInfoConfigMap_, key, isDelByBundleName);
        } else {
            defaultKeyMappingInfoConfigMap_[key] = config;
        }
        if (!isDelByBundleName) {
            DelayedSingleton<EventPublisher>::GetInstance()->SendGameKeyMappingConfigChangeNotify(gameKeyMappingInfo);
        }
        HILOGI("save default GameKeyMappingConfig success.");
        return GAME_CONTROLLER_SUCCESS;
    }
    HILOGE("save default GameKeyMappingConfig failed");
    return GAME_ERR_FAIL;
}

void KeyMappingConfigManager::DelFromMap(std::unordered_map<std::string, KeyMappingInfoConfig> &configMap,
                                         const std::string &key,
                                         bool isDelByBundleName)
{
    if (isDelByBundleName) {
        std::vector<std::string> keys;
        for (const auto &config: configMap) {
            if (config.first.find(key) == 0) {
                keys.push_back(config.first);
            }
        }
        for (const auto &keyInfo: keys) {
            configMap.erase(keyInfo);
        }
        return;
    }

    configMap.erase(key);
}

int32_t KeyMappingConfigManager::SetCustomGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfoForConst)
{
    GameKeyMappingInfo gameKeyMappingInfo = gameKeyMappingInfoForConst;
    if (!gameKeyMappingInfo.CheckParamValidForSetCustom() || !JsonUtils::IsUtf8(gameKeyMappingInfo.bundleName)) {
        HILOGE("CheckParamValidForSetCustom failed");
        return GAME_ERR_ARGUMENT_INVALID;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    std::unordered_map<std::string, KeyMappingInfoConfig> configMap;
    configMap.insert(customKeyMappingInfoConfigMap_.begin(), customKeyMappingInfoConfigMap_.end());
    KeyMappingInfoConfig config(gameKeyMappingInfo);
    std::string key = config.GetKeyForCustomKeyMapping();
    bool isDelByBundleName = gameKeyMappingInfo.IsDelByBundleNameWhenSetCustom();
    if (gameKeyMappingInfo.isOprDelete) {
        DelFromMap(configMap, key, isDelByBundleName);
    } else {
        configMap[key] = config;
    }
    if (SaveToConfigFile(configMap, GAME_CONTROLLER_SERVICE_ROOT + CUSTOM_KEY_MAPPING_CONFIG, true)) {
        if (gameKeyMappingInfo.isOprDelete) {
            DelFromMap(customKeyMappingInfoConfigMap_, key, isDelByBundleName);
        } else {
            customKeyMappingInfoConfigMap_[key] = config;
        }
        if (!isDelByBundleName) {
            DelayedSingleton<EventPublisher>::GetInstance()->SendGameKeyMappingConfigChangeNotify(gameKeyMappingInfo);
        }
        HILOGI("save custom GameKeyMappingConfig success.");
        return GAME_CONTROLLER_SUCCESS;
    }
    HILOGE("save custom GameKeyMappingConfig failed");
    return GAME_ERR_FAIL;
}

int32_t KeyMappingConfigManager::GetGameKeyMappingConfig(const GetGameKeyMappingInfoParam &param,
                                                         GameKeyMappingInfo &gameKeyMappingInfo)
{
    if (!param.CheckParamValid()) {
        HILOGE("CheckParamValidForGetGameKeyMappingConfig failed");
        return GAME_ERR_ARGUMENT_INVALID;
    }
    KeyMappingInfoConfig config;
    config.bundleName = param.bundleName;
    config.deviceType = static_cast<int32_t>(param.deviceType);

    std::lock_guard<std::mutex> lock(mutex_);

    // get the default config by bundleName and deviceType
    std::string defaultKey = config.GetKeyForDefaultKeyMapping();
    if (defaultKeyMappingInfoConfigMap_.find(defaultKey) != defaultKeyMappingInfoConfigMap_.end()) {
        KeyMappingInfoConfig defaultConfig = defaultKeyMappingInfoConfigMap_.at(defaultKey);
        gameKeyMappingInfo.defaultKeyToTouchMappings = defaultConfig.ConvertToDefaultKeyToTouchMapping();
    }

    std::string customKey = config.GetKeyForCustomKeyMapping();
    if (customKeyMappingInfoConfigMap_.find(customKey) != customKeyMappingInfoConfigMap_.end()) {
        KeyMappingInfoConfig customConfig = customKeyMappingInfoConfigMap_.at(customKey);
        gameKeyMappingInfo.customKeyToTouchMappings = customConfig.ConvertToCustomKeyToTouchMapping();
    }

    return GAME_CONTROLLER_SUCCESS;
}
}
}