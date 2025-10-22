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
#include "key_to_touch_manager.h"
#include "gamecontroller_utils.h"
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <filesystem>
#include <dirent.h>
#include <input_manager.h>

using json = nlohmann::json;
namespace OHOS {
namespace GameController {
namespace {
const std::string SUPPORT_KEYMAPPING_CFG = "/data/service/el1/public/for-all-app/gamecontroller_server/";
constexpr const char* CONFIG_FILE = "game_support_key_mapping.json";
constexpr const char* FIELD_BUNDLE_NAME = "bundleName";
constexpr const char* FIELD_VERSION = "version";
constexpr const char* FIELD_SUPPORT_DEVICE_TYPES = "deviceTypes";
const int32_t ALPHABETIC_KEYBOARD_TYPE = 2; // Full keyboard
}

KeyMappingSupportConfig::KeyMappingSupportConfig(const json &jsonObj)
{
    if (jsonObj.contains(FIELD_BUNDLE_NAME) && jsonObj.at(FIELD_BUNDLE_NAME).is_string()) {
        this->bundleName = jsonObj.at(FIELD_BUNDLE_NAME).get<std::string>();
    }
    if (jsonObj.contains(FIELD_VERSION) && jsonObj.at(FIELD_VERSION).is_string()) {
        this->version = jsonObj.at(FIELD_VERSION).get<std::string>();
    }
    if (jsonObj.contains(FIELD_SUPPORT_DEVICE_TYPES) && jsonObj.at(FIELD_SUPPORT_DEVICE_TYPES).is_array()) {
        this->deviceTypes = jsonObj.at(FIELD_SUPPORT_DEVICE_TYPES).get<std::unordered_set<int32_t>>();
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
    std::lock_guard<ffrt::mutex> lock(configMutex_);
    if (bundleName.empty()) {
        isSupportGameKeyMapping_ = false;
        return false;
    }

    bundleName_ = bundleName;
    bundleVersion_ = version;
    std::pair<bool, KeyMappingSupportConfig> result = GetKeyMappingSupportConfig();
    isSupportGameKeyMapping_ = result.first;
    keyMappingSupportConfig_ = result.second;
    SyncKeyMappingConfig();
    return isSupportGameKeyMapping_;
}

void KeyMappingService::BroadCastDeviceInfo(const DeviceInfo &deviceInfo)
{
    if (deviceInfo.deviceType == DeviceTypeEnum::GAME_MOUSE) {
        HILOGI("[BroadCastDeviceInfo] Device type is [%{public}d]. No key-mapping and broadcast. ",
               deviceInfo.deviceType);
        return;
    }
    DeviceInfo tempDeviceInfo = deviceInfo;
    if (deviceInfo.deviceType == DeviceTypeEnum::UNKNOWN) {
        if (deviceInfo.hasFullKeyBoard) {
            tempDeviceInfo.deviceType = GAME_KEY_BOARD;
        } else {
            return;
        }
    }

    handleQueue_->submit([tempDeviceInfo, this] {
        if (!DeviceIsSupportKeyMapping(tempDeviceInfo.deviceType)) {
            return;
        }
        if (tempDeviceInfo.status == 0) {
            // Device online
            if (loadTemplateCache_.count(tempDeviceInfo.deviceType) == 0 ||
                !loadTemplateCache_.at(tempDeviceInfo.deviceType)) {
                ExecuteGetGameKeyMapping(tempDeviceInfo.deviceType);
            } else {
                HILOGI("deviceType[%{public}d] has loaded key-mapping", tempDeviceInfo.deviceType);
            }
        }
        ExecuteBroadCastDeviceInfo(tempDeviceInfo);
    });
}

void KeyMappingService::ExecuteGetGameKeyMapping(DeviceTypeEnum deviceType)
{
    GetGameKeyMappingInfoParam param;
    param.bundleName = bundleName_;
    param.deviceType = deviceType;
    GameKeyMappingInfo gameKeyMappingInfo;
    int32_t result = DelayedSingleton<GameControllerServerClient>::GetInstance()->
        GetGameKeyMappingConfig(param, gameKeyMappingInfo);
    if (result == GAME_CONTROLLER_SUCCESS) {
        loadTemplateCache_[deviceType] = true;
        std::vector<KeyToTouchMappingInfo> mappingInfos;
        if (gameKeyMappingInfo.customKeyToTouchMappings.empty() &&
            gameKeyMappingInfo.defaultKeyToTouchMappings.empty()) {
            HILOGI("deviceType[%{public}d] doesn't have key-mapping config", deviceType);
            DelayedSingleton<KeyToTouchManager>::GetInstance()->UpdateTemplateConfig(deviceType,
                                                                                     mappingInfos);
            return;
        }

        HILOGI("deviceType[%{public}d] has key-mapping config", deviceType);
        if (gameKeyMappingInfo.customKeyToTouchMappings.empty()) {
            if (!gameKeyMappingInfo.defaultKeyToTouchMappings.empty()) {
                mappingInfos = gameKeyMappingInfo.defaultKeyToTouchMappings;
            }
        } else {
            mappingInfos = gameKeyMappingInfo.customKeyToTouchMappings;
        }

        DelayedSingleton<KeyToTouchManager>::GetInstance()->UpdateTemplateConfig(deviceType,
                                                                                 mappingInfos);
        return;
    }
    HILOGE("GetGameKeyMapping failed. deviceType[%{public}d]. result [%{private}d]",
           deviceType, result);
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

void KeyMappingService::BroadcastOpenTemplateConfig(const DeviceInfo &deviceInfo)
{
    handleQueue_->submit([deviceInfo, this] {
        ExecuteOpenTemplateConfig(deviceInfo);
    });
}

void KeyMappingService::ExecuteOpenTemplateConfig(const DeviceInfo &deviceInfo)
{
    if (!DeviceIsSupportKeyMapping(deviceInfo.deviceType)) {
        return;
    }
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

    content = nlohmann::json::parse(ifs, 0, false);
    if (content.is_discarded()) {
        HILOGE("parse [%{public}s] file failed.", canonicalPath);
        ifs.close();
        return std::make_pair(false, content);
    }

    if ((ifs.fail()) || (ifs.bad())) {
        HILOGE("load [%{public}s] file failed.", canonicalPath);
        ifs.close();
        return std::make_pair(false, content);
    }

    HILOGI("load game_support_key_mapping.json file success.");
    ifs.close();
    return std::make_pair(true, content);
}

void KeyMappingService::UpdateGameKeyMappingWhenTemplateChange(DeviceTypeEnum deviceType)
{
    handleQueue_->submit([deviceType, this] {
        if (!DeviceIsSupportKeyMapping(deviceType)) {
            return;
        }
        ExecuteGetGameKeyMapping(deviceType);
    });
}

std::pair<bool, KeyMappingSupportConfig> KeyMappingService::GetKeyMappingSupportConfig()
{
    std::pair<bool, KeyMappingSupportConfig> result;
    result.first = false;
    std::pair<bool, json> ret = ReadJsonFromFile(SUPPORT_KEYMAPPING_CFG + CONFIG_FILE);
    if (!ret.first) {
        return result;
    }

    json config = ret.second;
    if (config.empty()) {
        HILOGW("it's empty.");
        return result;
    }
    if (!config.is_array()) {
        HILOGW("it's not array json.");
        return result;
    }
    for (const auto &jsonObj: config) {
        KeyMappingSupportConfig gameConfig(jsonObj);
        if (gameConfig.bundleName == bundleName_) {
            result.first = true;
            result.second = gameConfig;
            HILOGI("bundleName[%{public}s] is support keymapping", bundleName_.c_str());
            return result;
        }
    }
    return result;
}

void KeyMappingService::ReloadKeyMappingSupportConfig()
{
    std::lock_guard<ffrt::mutex> lock(configMutex_);
    if (!isSupportGameKeyMapping_) {
        return;
    }
    HILOGI("ReloadKeyMappingSupportConfig");
    std::pair<bool, KeyMappingSupportConfig> result = GetKeyMappingSupportConfig();
    if (result.first) {
        keyMappingSupportConfig_ = result.second;
    } else {
        HILOGW("bundleName[%{public}s] switch to no support keymapping", bundleName_.c_str());
        isSupportGameKeyMapping_ = false;
        keyMappingSupportConfig_.deviceTypes.clear();
    }
    SyncKeyMappingConfig();
}

void KeyMappingService::SyncKeyMappingConfig()
{
    std::unordered_set<int32_t> deviceTypeSet(keyMappingSupportConfig_.deviceTypes.begin(),
                                              keyMappingSupportConfig_.deviceTypes.end());
    DelayedSingleton<KeyToTouchManager>::GetInstance()->SetSupportKeyMapping(isSupportGameKeyMapping_, deviceTypeSet);

}

bool KeyMappingService::DeviceIsSupportKeyMapping(DeviceTypeEnum deviceTypeEnum)
{
    std::lock_guard<ffrt::mutex> lock(configMutex_);
    if (!isSupportGameKeyMapping_) {
        // the game not support keymapping.
        return false;
    }
    if (keyMappingSupportConfig_.deviceTypes.count(deviceTypeEnum) == 0) {
        // the device type does not support keymapping.
        return false;
    }
    return true;
}

}
}