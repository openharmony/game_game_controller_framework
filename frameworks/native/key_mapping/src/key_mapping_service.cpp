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

#include "key_mapping_service.h"
#include "gamecontroller_errors.h"
#include "gamecontroller_server_client.h"
#include "gamecontroller_log.h"
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <filesystem>
#include <dirent.h>

using json = nlohmann::json;
namespace OHOS {
namespace GameController {
namespace {
const std::string SUPPORT_KEYMAPPING_CFG = "/data/service/el1/public/for-all-app/gamecontroller_server/";
constexpr const char* CONFIG_FILE = "game_support_key_mapping.json";
constexpr const char* FIELD_BUNDLE_NAME = "bundleName";
constexpr const char* FIELD_VERSION = "version";
}

KeyMappingSupportConfig::KeyMappingSupportConfig(const json &jsonObj)
{
    if (jsonObj.contains(FIELD_BUNDLE_NAME) && jsonObj.at(FIELD_BUNDLE_NAME).is_string()) {
        this->bundleName = jsonObj.at(FIELD_BUNDLE_NAME).get<std::string>();
    }
    if (jsonObj.contains(FIELD_VERSION) && jsonObj.at(FIELD_VERSION).is_string()) {
        this->version = jsonObj.at(FIELD_VERSION).get<std::string>();
    }
}

KeyMappingService::KeyMappingService()
{
    handleQueue_ = std::make_unique<ffrt::queue>("KeyMappingServiceHandleQueue",
                                                 ffrt::queue_attr().qos(ffrt::qos_default));
}

KeyMappingService::~KeyMappingService()
{
}

bool KeyMappingService::IsSupportGameKeyMapping(const std::string &bundleName, const std::string &version)
{
    if (bundleName.empty()) {
        isSupportGameKeyMapping_ = false;
        return false;
    }
    std::lock_guard<std::mutex> lock(isSupportGameKeyMappingMutex_);
    bundleName_ = bundleName;
    bundleVersion_ = version;
    isSupportGameKeyMapping_ = false;
    std::pair<bool, json> ret = ReadJsonFromFile(SUPPORT_KEYMAPPING_CFG + CONFIG_FILE);
    if (!ret.first) {
        return false;
    }

    json config = ret.second;
    if (config.empty()) {
        return false;
    }
    for (const auto &jsonObj: config) {
        KeyMappingSupportConfig gameConfig(jsonObj);
        if (gameConfig.bundleName == bundleName) {
            isSupportGameKeyMapping_ = true;
            HILOGI("bundleName[%{public}s] is support keymapping", bundleName.c_str());
            return true;
        }
    }
    return false;
}

void KeyMappingService::GetGameKeyMappingFromSa(const DeviceInfo &deviceInfo, bool isBroadCastDeviceInfo)
{
    if (isSupportGameKeyMapping_) {
        handleQueue_->submit([deviceInfo, isBroadCastDeviceInfo, this] {
            ExecuteGetGameKeyMapping(deviceInfo);
            if (isBroadCastDeviceInfo) {
                ExecuteBroadCastDeviceInfo(deviceInfo);
            }
        });
    }
}

void KeyMappingService::ExecuteGetGameKeyMapping(const DeviceInfo &deviceInfo)
{
    std::lock_guard<std::mutex> lock(isSupportGameKeyMappingMutex_);
    GetGameKeyMappingInfoParam param;
    param.bundleName = bundleName_;
    param.uniq = deviceInfo.uniq;
    param.deviceType = deviceInfo.deviceType;
    GameKeyMappingInfo gameKeyMappingInfo;
    HILOGI("GetGameKeyMapping. uniq[%{public}s]", deviceInfo.anonymizationUniq.c_str());
    int32_t result = DelayedSingleton<GameControllerServerClient>::GetInstance()->
        GetGameKeyMappingConfig(param, gameKeyMappingInfo);
    if (result == GAME_CONTROLLER_SUCCESS) {
        if (gameKeyMappingInfo.customKeyToTouchMappings.empty() &&
            gameKeyMappingInfo.defaultKeyToTouchMappings.empty()) {
            HILOGW("uniq[%{public}s] doesn't have key-mapping config", deviceInfo.anonymizationUniq.c_str());
            return;
        }
        HILOGI("uniq[%{public}s] has key-mapping config", deviceInfo.anonymizationUniq.c_str());
        if (gameKeyMappingInfo.customKeyToTouchMappings.empty()) {
            customKeyMappingInfoMap_.erase(deviceInfo.uniq);
        } else {
            customKeyMappingInfoMap_[deviceInfo.uniq] = KeyMappingInfo(gameKeyMappingInfo.customKeyToTouchMappings);
        }
        if (gameKeyMappingInfo.defaultKeyToTouchMappings.empty()) {
            defaultKeyMappingInfoMap_.erase(deviceInfo.deviceType);
        } else {
            defaultKeyMappingInfoMap_[deviceInfo.deviceType] = KeyMappingInfo(
                gameKeyMappingInfo.defaultKeyToTouchMappings);
        }
        return;
    }
    HILOGE("GetGameKeyMapping failed. uniq[%{public}s]. result [%{public}d]",
           deviceInfo.anonymizationUniq.c_str(), result);
}

void KeyMappingService::ExecuteBroadCastDeviceInfo(const DeviceInfo &deviceInfo)
{
    GameInfo gameInfo;
    gameInfo.bundleName = bundleName_;
    gameInfo.version = bundleVersion_;
    HILOGI("ExecuteBroadCastDeviceInfo");
    DelayedSingleton<GameControllerServerClient>::GetInstance()->BroadcastDeviceInfo(gameInfo,
                                                                                     deviceInfo);
}

std::pair<bool, KeyToTouchMappingInfo> KeyMappingService::GetGameKeyMappingFromCache(const DeviceInfo &deviceInfo,
                                                                                     const int32_t keyCode)
{
    std::lock_guard<std::mutex> lock(isSupportGameKeyMappingMutex_);
    std::pair<bool, KeyToTouchMappingInfo> pair;
    KeyMappingInfo info;

    /**
     * If a custom template exists, use the custom template firstly.
     */
    if (customKeyMappingInfoMap_.find(deviceInfo.uniq) != customKeyMappingInfoMap_.end()) {
        info = customKeyMappingInfoMap_[deviceInfo.uniq];
    } else {
        if (defaultKeyMappingInfoMap_.find(deviceInfo.deviceType) != defaultKeyMappingInfoMap_.end()) {
            pair.first = true;
            info = defaultKeyMappingInfoMap_[deviceInfo.deviceType];
        } else {
            pair.first = false;
            return pair;
        }
    }

    if (info.keyMappingInfoMap.find(keyCode) == info.keyMappingInfoMap.end()) {
        pair.first = false;
        return pair;
    }
    pair.first = true;
    pair.second = info.keyMappingInfoMap[keyCode];
    return pair;
}

void KeyMappingService::BroadcastOpenTemplateConfig(const DeviceInfo &deviceInfo)
{
    if (isSupportGameKeyMapping_) {
        handleQueue_->submit([deviceInfo, this] {
            ExecuteOpenTemplateConfig(deviceInfo);
        });
    }
}

void KeyMappingService::ExecuteOpenTemplateConfig(const DeviceInfo &deviceInfo)
{
    GameInfo gameInfo;
    gameInfo.bundleName = bundleName_;
    gameInfo.version = bundleVersion_;
    HILOGI("ExecuteOpenTemplateConfig");
    DelayedSingleton<GameControllerServerClient>::GetInstance()->BroadcastOpenTemplateConfig(gameInfo,
                                                                                             deviceInfo);
}

std::pair<bool, nlohmann::json> KeyMappingService::ReadJsonFromFile(const std::string &path)
{
    nlohmann::json content;
    char canonicalPath[PATH_MAX] = {};
    if (realpath(path.c_str(), canonicalPath) == nullptr) {
        HILOGW("realpath failed for [%{public}s]", path.c_str());
        return std::make_pair(false, content);
    }

    std::ifstream ifs(canonicalPath, std::ios::binary);
    if (!ifs.is_open()) {
        HILOGE("open [%{public}s] file failed.", canonicalPath);
        return std::make_pair(false, content);
    }

    ifs >> content;
    if ((ifs.fail()) || (ifs.bad())) {
        HILOGE("load [%{public}s] file failed.", canonicalPath);
        ifs.close();
        return std::make_pair(false, content);
    }

    HILOGI("load game_support_key_mapping.json file success.");
    ifs.close();
    return std::make_pair(true, content);
}
}
}