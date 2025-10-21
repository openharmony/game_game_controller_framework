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

#ifndef GAME_CONTROLLER_GAME_SUPPORT_KEY_MAPPING_MANAGER_H
#define GAME_CONTROLLER_GAME_SUPPORT_KEY_MAPPING_MANAGER_H

#include <memory>
#include <unordered_map>
#include <gamecontroller_keymapping_model.h>
#include <vector>
#include "nlohmann/json.hpp"
#include "singleton.h"

namespace OHOS {
namespace GameController {
struct GameSupportKeyMappingConfig {
    std::string bundleName;
    std::string version;
    std::vector<int32_t> supportedDeviceTypes;

    GameSupportKeyMappingConfig() = default;

    explicit GameSupportKeyMappingConfig(const nlohmann::json &jsonObj);

    explicit GameSupportKeyMappingConfig(const GameInfo &gameInfo)
    {
        bundleName = gameInfo.bundleName;
        version = gameInfo.version;
        supportedDeviceTypes = gameInfo.supportedDeviceTypes;
    }

    nlohmann::json ConvertToJson() const;
};

class GameSupportKeyMappingManager : public std::enable_shared_from_this<GameSupportKeyMappingManager> {
DECLARE_DELAYED_SINGLETON(GameSupportKeyMappingManager)

public:
    /**
     * Determines whether the game supports key mapping.
     * @param gameInfo Game Information
     * @param resultGameInfo Check Result
     * @return Interface Invoking Result
     */
    int32_t IsSupportGameKeyMapping(const GameInfo &gameInfo, GameInfo &resultGameInfo);

    /**
     * Synchronize games that support key mapping
     * @param If the value is true, full synchronization is performed. If the value is false,
     * only one record is updated.
     * @param gameInfos Synchronized game information
     * @return Interface Invoking Result
     */
    int32_t SyncSupportKeyMappingGames(bool isSyncAll, const std::vector<GameInfo> &gameInfos);

private:
    /**
     * Load the configuration from the JSON configuration file.
     */
    void LoadConfigFromJsonFile();

    /**
     * Save to Configuration File
     * @param configMap Configuration
     * @retrun true means success
     */
    bool SaveToConfigFile(const std::unordered_map<std::string, GameSupportKeyMappingConfig> &configMap);

private:
    std::mutex mutex_;

    /**
     * Cache game configuration
     * key is the bundleName
     */
    std::unordered_map<std::string, GameSupportKeyMappingConfig> configMap_;
};
}
}

#endif //GAME_CONTROLLER_GAME_SUPPORT_KEY_MAPPING_MANAGER_H
