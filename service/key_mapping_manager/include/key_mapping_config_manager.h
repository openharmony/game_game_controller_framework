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

#ifndef GAME_CONTROLLER_KEY_MAPPING_CONFIG_MANAGER_H
#define GAME_CONTROLLER_KEY_MAPPING_CONFIG_MANAGER_H

#include <string>
#include <cstdint>
#include <unordered_map>
#include <gamecontroller_client_model.h>
#include <gamecontroller_keymapping_model.h>
#include "nlohmann/json.hpp"
#include "singleton.h"

namespace OHOS {
namespace GameController {
struct DpadInfo {
    int32_t up = 0;
    int32_t down = 0;
    int32_t left = 0;
    int32_t right = 0;

    DpadInfo() = default;

    explicit DpadInfo(const nlohmann::json &jsonObj);

    explicit DpadInfo(const DpadKeyCodeEntity &dpadKeyCodeEntity)
    {
        up = dpadKeyCodeEntity.up;
        down = dpadKeyCodeEntity.down;
        left = dpadKeyCodeEntity.left;
        right = dpadKeyCodeEntity.right;
    }

    DpadKeyCodeEntity ConvertToDpadKeyCodeEntity() const
    {
        DpadKeyCodeEntity dpadKeyCodeEntity;
        dpadKeyCodeEntity.up = up;
        dpadKeyCodeEntity.down = down;
        dpadKeyCodeEntity.left = left;
        dpadKeyCodeEntity.right = right;
        return dpadKeyCodeEntity;
    }

    nlohmann::json ConvertToJson() const;
};

struct KeyMapping {
    KeyMapping() = default;

    explicit KeyMapping(const nlohmann::json &jsonObj);

    explicit KeyMapping(const KeyToTouchMappingInfo &keyToTouchMappingInfo)
    {
        mappingType = keyToTouchMappingInfo.mappingType;
        keyCode = keyToTouchMappingInfo.keyCode;
        xValue = keyToTouchMappingInfo.xValue;
        yValue = keyToTouchMappingInfo.yValue;
        dpadInfo = DpadInfo(keyToTouchMappingInfo.dpadKeyCodeEntity);
        combinationKeys = keyToTouchMappingInfo.combinationKeys;
        radius = keyToTouchMappingInfo.radius;
        skillRange = keyToTouchMappingInfo.skillRange;
        xStep = keyToTouchMappingInfo.xStep;
        yStep = keyToTouchMappingInfo.yStep;
        delayTime = keyToTouchMappingInfo.delayTime;
    }

    int32_t mappingType = 0;
    int32_t keyCode = 0;
    int32_t xValue = 0;
    int32_t yValue = 0;
    DpadInfo dpadInfo;
    std::vector<int32_t> combinationKeys;
    int32_t radius = 0;
    int32_t skillRange = 0;
    int32_t xStep = 0;
    int32_t yStep = 0;
    int32_t delayTime = 0;

    nlohmann::json ConvertToJson() const;

    KeyToTouchMappingInfo ConvertToKeyToTouchMappingInfo() const
    {
        KeyToTouchMappingInfo keyToTouchMappingInfo;
        keyToTouchMappingInfo.mappingType = static_cast<MappingTypeEnum>(mappingType);
        keyToTouchMappingInfo.xValue = xValue;
        keyToTouchMappingInfo.yValue = yValue;
        keyToTouchMappingInfo.keyCode = keyCode;
        keyToTouchMappingInfo.radius = radius;
        keyToTouchMappingInfo.skillRange = skillRange;
        keyToTouchMappingInfo.xStep = xStep;
        keyToTouchMappingInfo.yStep = yStep;
        keyToTouchMappingInfo.delayTime = delayTime;
        keyToTouchMappingInfo.combinationKeys = combinationKeys;
        keyToTouchMappingInfo.dpadKeyCodeEntity = dpadInfo.ConvertToDpadKeyCodeEntity();
        return keyToTouchMappingInfo;
    }

    int32_t GetJsonValue(const char* fieldName, int32_t defaultVal);
};

struct KeyMappingInfoConfig {
    KeyMappingInfoConfig() = default;

    explicit KeyMappingInfoConfig(const nlohmann::json &jsonObj, const bool isCustom);

    explicit KeyMappingInfoConfig(const GameKeyMappingInfo &gameKeyMappingInfo)
    {
        bundleName = gameKeyMappingInfo.bundleName;
        deviceType = static_cast<int32_t>(gameKeyMappingInfo.deviceType);
        for (const auto &keyMapping: gameKeyMappingInfo.customKeyToTouchMappings) {
            customKeyMappings.push_back(KeyMapping(keyMapping));
        }

        for (const auto &keyMapping: gameKeyMappingInfo.defaultKeyToTouchMappings) {
            defaultKeyMappings.push_back(KeyMapping(keyMapping));
        }
    };

    std::string bundleName;

    int32_t deviceType = 0;

    std::vector<KeyMapping> customKeyMappings;

    std::vector<KeyMapping> defaultKeyMappings;

    nlohmann::json ConvertToJson(const bool isCustom) const;

    std::vector<KeyToTouchMappingInfo> ConvertToCustomKeyToTouchMapping() const
    {
        std::vector<KeyToTouchMappingInfo> result;
        for (const auto &keyMapping: customKeyMappings) {
            result.push_back(keyMapping.ConvertToKeyToTouchMappingInfo());
        }
        return result;
    }

    std::vector<KeyToTouchMappingInfo> ConvertToDefaultKeyToTouchMapping() const
    {
        std::vector<KeyToTouchMappingInfo> result;
        for (const auto &keyMapping: defaultKeyMappings) {
            result.push_back(keyMapping.ConvertToKeyToTouchMappingInfo());
        }
        return result;
    }

    std::string GetKeyForDefaultKeyMapping()
    {
        if (deviceType == 0) {
            return bundleName + "_";
        }
        return bundleName + "_" + std::to_string(deviceType);
    }

    std::string GetKeyForCustomKeyMapping()
    {
        return GetKeyForDefaultKeyMapping();
    }
};

class KeyMappingConfigManager : public DelayedSingleton<KeyMappingConfigManager> {
DECLARE_DELAYED_SINGLETON(KeyMappingConfigManager)

public:
    /**
     * Set default game key mapping configuration
     * @param infos default settings for all game key mappings
     * @return Interface Invoking Result
     */
    int32_t SetDefaultGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfoForConst);

    /**
     * Set custom game key mapping configuration
     * @param config Configuration
     * @return Interface Invoking Result
     */
    int32_t SetCustomGameKeyMappingConfig(const GameKeyMappingInfo &gameKeyMappingInfoForConst);

    /**
    * Getting the Game Key Mapping Configuration
    * @param param the request param
    * @param gameKeyMappingInfo Game Key Mapping Configuration
    * @return Interface Invoking Result
    */
    int32_t GetGameKeyMappingConfig(const GetGameKeyMappingInfoParam &param,
                                    GameKeyMappingInfo &gameKeyMappingInfo);

private:
    /**
     * Save to Configuration File
     * @param configMap Configuration
     * @param filePath the JSON configuration file.
     * @param isCustom true means it's custom config
     * @retrun true means success
     */
    bool SaveToConfigFile(const std::unordered_map<std::string, KeyMappingInfoConfig> &configMap,
                          const std::string &filePath, const bool isCustom);

    /**
     * Load the configuration from the JSON configuration file.
     */
    void LoadConfigFromJsonFile();

    std::unordered_map<std::string, KeyMappingInfoConfig> LoadConfigFromJsonFile(const std::string &filePath,
                                                                                 const bool isCustom);

    void DelFromMap(std::unordered_map<std::string, KeyMappingInfoConfig> &configMap, const std::string &key,
                    bool isDelByBundleName);

private:
    /**
     * Default Cache Configuration
     * key: bundleName + _ + deviceType
     * value: KeyMappingInfoConfig
     */
    std::unordered_map<std::string, KeyMappingInfoConfig> defaultKeyMappingInfoConfigMap_;

    /**
     * Customized Cache Configuration
     * key: bundleName + _ + deviceType
     * value: KeyMappingInfoConfig
     */
    std::unordered_map<std::string, KeyMappingInfoConfig> customKeyMappingInfoConfigMap_;

    std::mutex mutex_;
};
}
}
#endif //GAME_CONTROLLER_KEY_MAPPING_CONFIG_MANAGER_H
