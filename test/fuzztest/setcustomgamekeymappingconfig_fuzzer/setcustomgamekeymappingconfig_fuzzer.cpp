/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cstdint>
#include <gamecontroller_log.h>
#include "setcustomgamekeymappingconfig_fuzzer.h"
#include "gamecontroller_constants.h"
#include "key_mapping_config_manager.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t MAPPING_TYPE_NUM = 10;
const int32_t DEVICE_TYPE_NUM = 4;
const int32_t KEY_UP = 1;
const int32_t KEY_DOWN = 2;
const int32_t KEY_LEFT = 3;
const int32_t KEY_RIGHT = 4;
const int32_t COMBINATION_FIRST_KEY = 5;
const int32_t COMBINATION_SECOND_KEY = 6;
}

void SetCustomGameKeyMappingConfig(const uint8_t* rawData, size_t size)
{
    GameKeyMappingInfo gameKeyMappingInfo;
    gameKeyMappingInfo.bundleName = std::string(reinterpret_cast<const char*>(rawData), size)
        .substr(0, MAX_BUNDLE_NAME_LENGTH);
    gameKeyMappingInfo.isOprDelete = static_cast<bool>(*rawData);
    int32_t deviceType = static_cast<int32_t >(*rawData);
    gameKeyMappingInfo.deviceType = static_cast<DeviceTypeEnum>(deviceType % DEVICE_TYPE_NUM);
    std::vector<KeyToTouchMappingInfo> defaultKeyToTouchMappings;
    for (int32_t index = 0; index < MAPPING_TYPE_NUM; index++) {
        KeyToTouchMappingInfo keyToTouchMappingInfo;
        keyToTouchMappingInfo.keyCode = static_cast<int32_t>(*rawData + index);
        keyToTouchMappingInfo.xValue = static_cast<int32_t>(*rawData);
        keyToTouchMappingInfo.yValue = static_cast<int32_t>(*rawData);
        keyToTouchMappingInfo.mappingType = static_cast<MappingTypeEnum>(
			static_cast<int32_t>(*rawData) % MAPPING_TYPE_NUM);
        keyToTouchMappingInfo.radius = static_cast<int32_t>(*rawData);
        keyToTouchMappingInfo.skillRange = static_cast<int32_t>(*rawData);
        keyToTouchMappingInfo.xStep = static_cast<int32_t>(*rawData);
        keyToTouchMappingInfo.yStep = static_cast<int32_t>(*rawData);
        std::vector<int32_t> combinationKeys;
        combinationKeys.push_back(static_cast<int32_t>(*rawData) + index + COMBINATION_FIRST_KEY);
        combinationKeys.push_back(static_cast<int32_t>(*rawData) + index + COMBINATION_SECOND_KEY);
        keyToTouchMappingInfo.combinationKeys = combinationKeys;
        DpadKeyCodeEntity dpadKeyCodeEntity;
        dpadKeyCodeEntity.up = static_cast<int32_t>(*rawData) + index + KEY_UP;
        dpadKeyCodeEntity.down = static_cast<int32_t>(*rawData) + index + KEY_DOWN;
        dpadKeyCodeEntity.left = static_cast<int32_t>(*rawData) + index + KEY_LEFT;
        dpadKeyCodeEntity.right = static_cast<int32_t>(*rawData) + index + KEY_RIGHT;
        keyToTouchMappingInfo.dpadKeyCodeEntity = dpadKeyCodeEntity;
        defaultKeyToTouchMappings.push_back(keyToTouchMappingInfo);
    }
    gameKeyMappingInfo.customKeyToTouchMappings = defaultKeyToTouchMappings;
    gameKeyMappingInfo.defaultKeyToTouchMappings = defaultKeyToTouchMappings;
    GameInfo gameInfo;
    gameInfo.bundleName = gameKeyMappingInfo.bundleName;
    DeviceInfo deviceInfo;
    deviceInfo.deviceType = gameKeyMappingInfo.deviceType;
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetDefaultGameKeyMappingConfig(gameKeyMappingInfo);
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->SetCustomGameKeyMappingConfig(gameKeyMappingInfo);
    GameKeyMappingInfo result;
    GetGameKeyMappingInfoParam param;
    param.bundleName = gameKeyMappingInfo.bundleName;
    param.deviceType = gameKeyMappingInfo.deviceType;
    DelayedSingleton<KeyMappingConfigManager>::GetInstance()->GetGameKeyMappingConfig(param, result);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GameController::SetCustomGameKeyMappingConfig(data, size);
    return 0;
}