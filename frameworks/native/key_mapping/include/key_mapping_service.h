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

struct KeyMappingInfo {
    std::unordered_map<int32_t, KeyToTouchMappingInfo> keyMappingInfoMap;

    KeyMappingInfo() = default;

    KeyMappingInfo(const std::vector<KeyToTouchMappingInfo> mappingInfo)
    {
        for (const auto &item: mappingInfo) {
            keyMappingInfoMap[item.keyCode] = item;
            HILOGI("KeyMappingInfo [%{public}s]", item.GetKeyToTouchMappingInfoDesc().c_str());
        }
    }
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
     * Get KeyMapping config from sa
     * @param deviceInfo DeviceInfo
     * @param isBroadCastDeviceInfo Indicates whether to broadcast device information.
     */
    void GetGameKeyMappingFromSa(const DeviceInfo &deviceInfo, const bool isBroadCastDeviceInfo);

    /**
     * Get KeyMapping config from local cache
     * @param deviceInfo DeviceInfo
     * @param keyCode key code
     * @return GameKeyMappingInfo
     */
    std::pair<bool, KeyToTouchMappingInfo> GetGameKeyMappingFromCache(const DeviceInfo &deviceInfo,
                                                                      const int32_t keyCode);

    /**
     * Broadcast Opening Template Configuration Page
     * @param deviceInfo DeviceInfo
     */
    void BroadcastOpenTemplateConfig(const DeviceInfo &deviceInfo);

private:
    /**
     * Get KeyMapping config
     * @param deviceInfo DeviceInfo
     */
    void ExecuteGetGameKeyMapping(const DeviceInfo &deviceInfo);

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
    std::mutex isSupportGameKeyMappingMutex_;

    /**
     * handle queue
     */
    std::unique_ptr<ffrt::queue> handleQueue_{nullptr};

    bool isSupportGameKeyMapping_{false};

    std::string bundleName_;

    std::string bundleVersion_;

    std::unordered_map<std::string, KeyMappingInfo> customKeyMappingInfoMap_;

    std::unordered_map<DeviceTypeEnum, KeyMappingInfo> defaultKeyMappingInfoMap_;
};
}
}

#endif //GAME_CONTROLLER_FRAMEWORK_KEY_MAPPING_SERVICE_H
