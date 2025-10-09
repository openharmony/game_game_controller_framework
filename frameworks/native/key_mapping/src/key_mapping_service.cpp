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

    bundleName_ = bundleName;
    bundleVersion_ = version;
    isSupportGameKeyMapping_ = false;
    std::pair<bool, json> ret = ReadJsonFromFile(SUPPORT_KEYMAPPING_CFG + CONFIG_FILE);
    if (!ret.first) {
        return false;
    }

    json config = ret.second;
    if (config.empty()) {
        HILOGW("it's empty.");
        return false;
    }
    if (!config.is_array()) {
        HILOGW("it's not array json.");
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

void KeyMappingService::BroadCastDeviceInfo(const DeviceInfo &deviceInfo)
{
    if (!isSupportGameKeyMapping_) {
        return;
    }
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

void KeyMappingService::ExecuteGetGameKeyMapping(const DeviceTypeEnum deviceType)
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
        ExecuteGetGameKeyMapping(deviceType);
    });
}

}
}