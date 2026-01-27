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
#include <iostream>
#include <fstream>
#include <cstdint>
#include <nlohmann/json.hpp>
#include "gamecontroller_constants.h"
#include "device_manager.h"
#include "gamecontroller_log.h"
#include "gamecontroller_errors.h"
#include "json_utils.h"

using json = nlohmann::json;

namespace OHOS {
namespace GameController {
namespace {
const char* PRODUCT = "Product";
const char* TYPE = "Type";
const char* VENDOR = "Vendor";
const char* NAME_PREFIX = "NamePrefix";
const char* DEVICE_CONFIG = "device_config.json";
const int32_t MAX_DEVICE_NUMBER = 10000;
/**
 * The device type to be identified can be configured. Currently, only GamePad is supported.
 */
const static std::unordered_map<DeviceTypeEnum, std::string> DEVICE_TYPE_LUT{
    {DeviceTypeEnum::GAME_PAD,        "GamePad"},
    {DeviceTypeEnum::HOVER_TOUCH_PAD, "HoverTouchPad"},
    {DeviceTypeEnum::GAME_KEY_BOARD,  "KeyBoard"},
    {DeviceTypeEnum::GAME_MOUSE,      "Mouse"}
};
}

IdentifyDeviceInfo::IdentifyDeviceInfo()
{
}

IdentifyDeviceInfo::IdentifyDeviceInfo(const json &jsonObj)
{
    if (jsonObj.contains(PRODUCT) && jsonObj.at(PRODUCT).is_number_integer()) {
        this->productId = jsonObj.at(PRODUCT).get<int32_t>();
    }
    if (jsonObj.contains(VENDOR) && jsonObj.at(VENDOR).is_number_integer()) {
        this->vendor = jsonObj.at(VENDOR).get<int32_t>();
    }
    if (jsonObj.contains(TYPE) && jsonObj.at(TYPE).is_number_integer()) {
        this->deviceType = jsonObj.at(TYPE).get<int32_t>();
    }
    if (jsonObj.contains(NAME_PREFIX) && jsonObj.at(NAME_PREFIX).is_string()) {
        this->namePrefix = jsonObj.at(NAME_PREFIX).get<std::string>();
    }
}

bool IdentifyDeviceInfo::IsMatched(const DeviceInfo &device) const
{
    if ((device.product == this->productId && device.vendor == this->vendor)) {
        return true;
    }
    return device.name.find(this->namePrefix) == 0;
}

nlohmann::json IdentifyDeviceInfo::ConvertToJson() const
{
    json jsonContent;
    jsonContent[PRODUCT] = this->productId;
    jsonContent[VENDOR] = this->vendor;
    jsonContent[TYPE] = this->deviceType;
    jsonContent[NAME_PREFIX] = this->namePrefix;
    return jsonContent;
}

DeviceManager::DeviceManager()
{
    LoadDeviceCacheFile();
}

DeviceManager::~DeviceManager()
{
}

void DeviceManager::BuildDeviceInfoWithType(const DeviceInfo &src, std::vector<DeviceInfo> &result, DeviceTypeEnum type)
{
    DeviceInfo device(src);
    device.deviceType = type;
    result.emplace_back(device);
}

int32_t DeviceManager::DeviceIdentify(const std::vector<DeviceInfo> &devices, std::vector<DeviceInfo> &result)
{
    if (devices.empty()) {
        HILOGI("Request devices is empty.");
        return GAME_CONTROLLER_SUCCESS;
    }

    std::lock_guard<std::mutex> lock(mMutex_);
    if (identifiedDevices_.empty()) {
        HILOGI("No identified device.");
        result.insert(result.begin(), devices.begin(), devices.end());
        for (auto &item: result) {
            item.deviceType = DeviceTypeEnum::UNKNOWN;
        }
        return GAME_CONTROLLER_SUCCESS;
    }

    for (const auto &device: devices) {
        // If the fuzzy match fails, the exact match is performed.
        auto it = find_if(identifiedDevices_.begin(), identifiedDevices_.end(),
                          [device](const IdentifyDeviceInfo authed) { return authed.IsMatched(device); });
        if (it != identifiedDevices_.end()) {
            BuildDeviceInfoWithType(device, result, static_cast<DeviceTypeEnum>((*it).deviceType));
            HILOGI("Device identified, name[%{public}s], type[%{public}d]", device.name.c_str(), (*it).deviceType);
            continue;
        }
        HILOGI("Un-authed identified, name[%{public}s]", device.name.c_str());
        BuildDeviceInfoWithType(device, result, DeviceTypeEnum::UNKNOWN);
    }
    return GAME_CONTROLLER_SUCCESS;
}

void DeviceManager::LoadDeviceCacheFile()
{
    std::lock_guard<std::mutex> lock(mMutex_);
    if (!JsonUtils::IsFileExist(GAME_CONTROLLER_SERVICE_ROOT + DEVICE_CONFIG)) {
        HILOGI("[%{public}s] does not exist in service directory.", DEVICE_CONFIG);
        if (!JsonUtils::CopyFile(GAME_CONTROLLER_ETC_ROOT + DEVICE_CONFIG,
                                 GAME_CONTROLLER_SERVICE_ROOT + DEVICE_CONFIG,
                                 false)) {
            HILOGE("[%{public}s] copy to service directory failed.", DEVICE_CONFIG);
            return;
        }
    }
    std::pair<bool, json> ret = JsonUtils::ReadJsonFromFile(GAME_CONTROLLER_SERVICE_ROOT + DEVICE_CONFIG);
    if (!ret.first) {
        HILOGW("load [%{public}s] file failed.", DEVICE_CONFIG);
        return;
    }
    LoadFromJson(ret.second);
}

void DeviceManager::LoadFromJson(const json &config)
{
    if (config.empty()) {
        HILOGW("config is empty.");
        return;
    }

    std::vector<IdentifyDeviceInfo> devices;
    for (const auto &[gameType, typeName]: DEVICE_TYPE_LUT) {
        if (config.contains(typeName) && config.at(typeName).is_array()) {
            auto dataJson = config[typeName.c_str()];
            for (const auto &j: dataJson) {
                IdentifyDeviceInfo device(j);
                device.deviceType = static_cast<int32_t>(gameType);
                devices.emplace_back(device);
            }
        } else {
            HILOGW("type[%{public}d] is not json array.", gameType);
            continue;
        }
    }
    identifiedDevices_.insert(identifiedDevices_.end(), devices.begin(), devices.end());
    HILOGI("identifiedDevices size[%{public}lu].", identifiedDevices_.size());
}

int32_t DeviceManager::SyncIdentifiedDeviceInfos(const std::vector<IdentifiedDeviceInfo> &deviceInfos)
{
    if (deviceInfos.size() > MAX_DEVICE_NUMBER) {
        HILOGE("Over max device number.identifiedDevices size[%{public}lu].", deviceInfos.size());
        return GAME_ERR_ARRAY_MAXSIZE;
    }
    std::lock_guard<std::mutex> lock(mMutex_);
    json jsonContent;
    std::vector<IdentifyDeviceInfo> devices;
    for (const auto &device: deviceInfos) {
        if (!device.CheckParamValid() || !JsonUtils::IsUtf8(device.name)) {
            HILOGE("param is invalid");
            return GAME_ERR_ARGUMENT_INVALID;
        }
        if (DEVICE_TYPE_LUT.find(device.deviceType) != DEVICE_TYPE_LUT.end()) {
            IdentifyDeviceInfo identifyDeviceInfo(device);
            jsonContent[DEVICE_TYPE_LUT.at(device.deviceType)].push_back(identifyDeviceInfo.ConvertToJson());
            devices.push_back(identifyDeviceInfo);
        } else {
            HILOGW("discard unknown device type, deviceName is [%{public}s]", device.name.c_str());
            continue;
        }
    }

    return SaveToConfigFile(jsonContent, devices);
}

int32_t DeviceManager::SaveToConfigFile(json &jsonContent, std::vector<IdentifyDeviceInfo> &devices)
{
    if (devices.empty()) {
        jsonContent = json::object();
    }
    if (JsonUtils::WriteFileFromJson(GAME_CONTROLLER_SERVICE_ROOT + DEVICE_CONFIG, jsonContent)) {
        HILOGI("SyncIdentifiedDeviceInfos success. identifiedDevices size[%{public}lu].", devices.size());
        identifiedDevices_.clear();
        identifiedDevices_.insert(identifiedDevices_.end(), devices.begin(), devices.end());
        return GAME_CONTROLLER_SUCCESS;
    } else {
        HILOGE("SyncIdentifiedDeviceInfos failed.");
        return GAME_ERR_FAIL;
    }
}
} // namespace GameController
} // namespace OHOS

