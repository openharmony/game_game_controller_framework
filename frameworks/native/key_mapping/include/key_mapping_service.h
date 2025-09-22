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

#ifndef GAME_CONTROLLER_FRAMEWORK_KEY_MAPPING_SERVICE_H
#define GAME_CONTROLLER_FRAMEWORK_KEY_MAPPING_SERVICE_H

#include <singleton.h>
#include <unordered_map>
#include "gamecontroller_keymapping_model.h"
#include "ffrt.h"
#include "gamecontroller_log.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace GameController {
struct KeyMappingSupportConfig {
    std::string bundleName;
    std::string version;

    KeyMappingSupportConfig() = default;

    explicit KeyMappingSupportConfig(const nlohmann::json &jsonObj);
};

class KeyMappingService : public DelayedSingleton<KeyMappingService> {
DECLARE_DELAYED_SINGLETON(KeyMappingService)

public:
    /**
     * Check whether the game supports input-to-touch control.
     * It's only called once.
     * @param bundleName bundleName
     * @param version version
     * @return true means the game supports input-to-touch control.
     */
    bool IsSupportGameKeyMapping(const std::string &bundleName, const std::string &version);

    /**
     * Broadcast device status and get KeyMapping config from sa if online
     * @param deviceInfo DeviceInfo
     */
    void BroadCastDeviceInfo(const DeviceInfo &deviceInfo);

    /**
     * When the template is changed, update the key mapping.
     * @param deviceType DeviceTypeEnum
     */
    void UpdateGameKeyMappingWhenTemplateChange(DeviceTypeEnum deviceType);

    /**
     * Broadcast Opening Template Configuration Page
     * @param deviceInfo DeviceInfo
     */
    void BroadcastOpenTemplateConfig(const DeviceInfo &deviceInfo);

private:

    /**
     * Get KeyMapping config
     * @param deviceType DeviceTypeEnum
     */
    void ExecuteGetGameKeyMapping(const DeviceTypeEnum deviceType);

    /**
     * Broadcast device information.
     * @param deviceInfo DeviceInfo
     */
    void ExecuteBroadCastDeviceInfo(const DeviceInfo &deviceInfo);

    /**
     * Broadcast Opening Template Configuration Page
     * @param deviceInfo DeviceInfo
     */
    void ExecuteOpenTemplateConfig(const DeviceInfo &deviceInfo);

    /**
     * Read the JSON object from the file.
     * @param path File Path
     * @return pair.first indicates whether the operation is successful,
     * and pair.second indicates the JSON content to be read.
     */
    std::pair<bool, nlohmann::json> ReadJsonFromFile(const std::string &path);

private:
    /**
     * handle queue
     */
    std::unique_ptr<ffrt::queue> handleQueue_{nullptr};

    bool isSupportGameKeyMapping_{false};

    std::string bundleName_;

    std::string bundleVersion_;

    std::unordered_map<DeviceTypeEnum, bool> loadTemplateCache_;
};
}
}
#endif //GAME_CONTROLLER_FRAMEWORK_KEY_MAPPING_SERVICE_H
