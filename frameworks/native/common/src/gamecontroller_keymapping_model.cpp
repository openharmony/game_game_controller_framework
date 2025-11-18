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

#include <unordered_map>
#include "gamecontroller_log.h"
#include "gamecontroller_keymapping_model.h"

namespace OHOS {
namespace GameController {

namespace {
typedef bool(GameKeyMappingInfo::*CheckHandler)(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

const size_t MAX_SINGLE_KEY_MAPPING_SIZE = 50;
const size_t MAX_COMBINATION_KEY_MAPPING_SIZE = 30;
const size_t MAX_SKILL_KEY_MAPPING_SIZE = 10;
const size_t MAX_SINGLE_KEY_SIZE_FOR_HOVER_TOUCH_PAD = 2;
const int32_t MAX_DELAY_TIME = 5;
std::unordered_map<MappingTypeEnum, CheckHandler> checkKeyMappingHandlerMap = {
    {MappingTypeEnum::SINGE_KEY_TO_TOUCH,               &GameKeyMappingInfo::CheckSingleKey},
    {MappingTypeEnum::COMBINATION_KEY_TO_TOUCH,         &GameKeyMappingInfo::CheckCombinationKey},
    {MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH, &GameKeyMappingInfo::CheckMouseRightKeyWalking},
    {MappingTypeEnum::DPAD_KEY_TO_TOUCH,                &GameKeyMappingInfo::CheckDpadKey},
    {MappingTypeEnum::SKILL_KEY_TO_TOUCH,               &GameKeyMappingInfo::CheckSkillKey},
    {MappingTypeEnum::OBSERVATION_KEY_TO_TOUCH,         &GameKeyMappingInfo::CheckObservationKey},
    {MappingTypeEnum::MOUSE_OBSERVATION_TO_TOUCH,       &GameKeyMappingInfo::CheckMouseObservation},
    {MappingTypeEnum::KEY_BOARD_OBSERVATION_TO_TOUCH,   &GameKeyMappingInfo::CheckKeyBoardObservation},
    {MappingTypeEnum::CROSSHAIR_KEY_TO_TOUCH,           &GameKeyMappingInfo::CheckCrosshairKey},
    {MappingTypeEnum::MOUSE_LEFT_FIRE_TO_TOUCH,         &GameKeyMappingInfo::CheckMouseLeftFire},
    {MappingTypeEnum::MOUSE_RIGHT_KEY_CLICK_TO_TOUCH,   &GameKeyMappingInfo::CheckMouseRightKeyClick}
};
}

bool GameKeyMappingInfo::CheckSingleKey(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter)
{
    if (++parameter.keyMappingNumber[MappingTypeEnum::SINGE_KEY_TO_TOUCH] > MAX_SINGLE_KEY_MAPPING_SIZE) {
        HILOGE("numbers of SINGE_KEY_TO_TOUCH have exceeded its limit[%{public}d]",
               static_cast<int32_t>(MAX_SINGLE_KEY_MAPPING_SIZE));
        return false;
    }
    if (parameter.uniqKeyCodeMap.find(currentKeyMapping.keyCode) != parameter.uniqKeyCodeMap.end()) {
        HILOGE("keycode has been used");
        return false;
    }
    parameter.uniqKeyCodeMap[currentKeyMapping.keyCode] = currentKeyMapping.mappingType;

    currentKeyMapping.SetDpadInfoToDefault();
    currentKeyMapping.combinationKeys.clear();
    currentKeyMapping.SetSkillRangeRadiusToDefault();
    currentKeyMapping.SetStepToDefault();
    return true;
}

bool GameKeyMappingInfo::CheckCombinationKey(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter)
{
    if (currentKeyMapping.combinationKeys.size() != MAX_COMBINATION_KEYS) {
        HILOGE("keycode numbers in combinationKeys have exceeded its limit[%{public}d]",
               static_cast<int32_t>(MAX_COMBINATION_KEYS));
        return false;
    }
    if (++parameter.keyMappingNumber[MappingTypeEnum::COMBINATION_KEY_TO_TOUCH] > MAX_COMBINATION_KEY_MAPPING_SIZE) {
        HILOGE("numbers of COMBINATION_KEY_TO_TOUCH have exceeded its limit[%{public}d]",
               static_cast<int32_t>(MAX_COMBINATION_KEY_MAPPING_SIZE));
        return false;
    }
    if (currentKeyMapping.combinationKeys[COMBINATION_FIRST_KEYCODE_IDX] ==
        currentKeyMapping.combinationKeys[COMBINATION_LAST_KEYCODE_IDX]) {
        HILOGE("keycode in combinationKeys repeats");
        return false;
    }
    auto first = parameter.combinationKeysMap.find(currentKeyMapping.combinationKeys[COMBINATION_FIRST_KEYCODE_IDX]);
    auto second = parameter.combinationKeysMap.find(currentKeyMapping.combinationKeys[COMBINATION_LAST_KEYCODE_IDX]);
    if ((first != parameter.combinationKeysMap.end() &&
        first->second == currentKeyMapping.combinationKeys[COMBINATION_LAST_KEYCODE_IDX]) ||
        (second != parameter.combinationKeysMap.end() &&
            second->second == currentKeyMapping.combinationKeys[COMBINATION_FIRST_KEYCODE_IDX])) {
        HILOGE("keycodes in combinationKeys have been used");
        return false;
    }
    parameter.combinationKeysMap[currentKeyMapping.combinationKeys[COMBINATION_FIRST_KEYCODE_IDX]] =
        currentKeyMapping.combinationKeys[COMBINATION_LAST_KEYCODE_IDX];

    currentKeyMapping.keyCode = 0;
    currentKeyMapping.SetDpadInfoToDefault();
    currentKeyMapping.SetSkillRangeRadiusToDefault();
    currentKeyMapping.SetStepToDefault();
    return true;
}

bool GameKeyMappingInfo::CheckDpadKey(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter)
{
    if (++parameter.keyMappingNumber[MappingTypeEnum::DPAD_KEY_TO_TOUCH] > 1) {
        HILOGE("numbers of DPAD_KEY_TO_TOUCH have exceeded its limit[1]");
        return false;
    }
    if (!IsDpadKeyCodeUniq(currentKeyMapping, parameter)) {
        return false;
    }

    currentKeyMapping.keyCode = 0;
    currentKeyMapping.combinationKeys.clear();
    currentKeyMapping.skillRange = 0;
    currentKeyMapping.SetStepToDefault();
    return true;
}

bool GameKeyMappingInfo::CheckMouseRightKeyWalking(KeyToTouchMappingInfo &currentKeyMapping,
                                                   ParameterByCheck &parameter)
{
    if (++parameter.keyMappingNumber[MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH] > 1) {
        HILOGE("numbers of MOUSE_RIGHT_KEY_WALKING_TO_TOUCH have exceeded its limit[1]");
        return false;
    }
    if (parameter.uniqRightButtonMappingType != -1) {
        HILOGE("mouse right button has been used in MOUSE_OBSERVATION_TO_TOUCH or MOUSE_RIGHT_KEY_CLICK_TO_TOUCH");
        return false;
    }
    if (currentKeyMapping.delayTime < 0 || currentKeyMapping.delayTime > MAX_DELAY_TIME) {
        HILOGE("MOUSE_RIGHT_KEY_WALKING_TO_TOUCH's delayTime must be between 0 and 5");
        return false;
    }
    parameter.uniqRightButtonMappingType = MappingTypeEnum::MOUSE_RIGHT_KEY_WALKING_TO_TOUCH;

    currentKeyMapping.keyCode = 0;
    currentKeyMapping.SetDpadInfoToDefault();
    currentKeyMapping.combinationKeys.clear();
    currentKeyMapping.skillRange = 0;
    currentKeyMapping.SetStepToDefault();
    return true;
}

bool GameKeyMappingInfo::CheckSkillKey(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter)
{
    if (currentKeyMapping.skillRange <= 0 || currentKeyMapping.radius <= 0) {
        HILOGE("skillRange and radius must be greater than 0");
        return false;
    }
    if (++parameter.keyMappingNumber[MappingTypeEnum::SKILL_KEY_TO_TOUCH] > MAX_SKILL_KEY_MAPPING_SIZE) {
        HILOGE("numbers of SKILL_KEY_TO_TOUCH have exceeded its limit[%{public}d]",
               static_cast<int32_t>(MAX_SKILL_KEY_MAPPING_SIZE));
        return false;
    }
    if (parameter.uniqKeyCodeMap.find(currentKeyMapping.keyCode) != parameter.uniqKeyCodeMap.end()) {
        HILOGE("keycode has been used.");
        return false;
    }
    parameter.uniqKeyCodeMap[currentKeyMapping.keyCode] = currentKeyMapping.mappingType;

    currentKeyMapping.SetDpadInfoToDefault();
    currentKeyMapping.combinationKeys.clear();
    currentKeyMapping.SetStepToDefault();
    return true;
}

bool GameKeyMappingInfo::CheckObservationKey(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter)
{
    if (!IsStepValid(currentKeyMapping)) {
        return false;
    }
    if (++parameter.keyMappingNumber[MappingTypeEnum::OBSERVATION_KEY_TO_TOUCH] > 1) {
        HILOGE("numbers of OBSERVATION_KEY_TO_TOUCH have exceeded its limit[1]");
        return false;
    }
    if (parameter.uniqKeyCodeMap.find(currentKeyMapping.keyCode) != parameter.uniqKeyCodeMap.end()) {
        HILOGE("keycode has been used.");
        return false;
    }
    parameter.uniqKeyCodeMap[currentKeyMapping.keyCode] = currentKeyMapping.mappingType;

    currentKeyMapping.SetDpadInfoToDefault();
    currentKeyMapping.combinationKeys.clear();
    currentKeyMapping.SetSkillRangeRadiusToDefault();
    return true;
}

bool GameKeyMappingInfo::CheckMouseObservation(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter)
{
    if (!IsStepValid(currentKeyMapping)) {
        return false;
    }
    if (++parameter.keyMappingNumber[MappingTypeEnum::MOUSE_OBSERVATION_TO_TOUCH] > 1) {
        HILOGE("numbers of MOUSE_OBSERVATION_TO_TOUCH have exceeded its limit[1]");
        return false;
    }
    if (parameter.uniqRightButtonMappingType != -1) {
        HILOGE(
            "mouse right button has been used in MOUSE_RIGHT_KEY_WALKING_TO_TOUCH or MOUSE_RIGHT_KEY_CLICK_TO_TOUCH");
        return false;
    }
    parameter.uniqRightButtonMappingType = MappingTypeEnum::MOUSE_OBSERVATION_TO_TOUCH;

    currentKeyMapping.keyCode = 0;
    currentKeyMapping.SetDpadInfoToDefault();
    currentKeyMapping.combinationKeys.clear();
    currentKeyMapping.SetSkillRangeRadiusToDefault();
    return true;
}

bool GameKeyMappingInfo::CheckKeyBoardObservation(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter)
{
    if (!IsStepValid(currentKeyMapping)) {
        return false;
    }
    if (++parameter.keyMappingNumber[MappingTypeEnum::KEY_BOARD_OBSERVATION_TO_TOUCH] > 1) {
        HILOGE("numbers of KEY_BOARD_OBSERVATION_TO_TOUCH have exceeded its limit[1]");
        return false;
    }
    if (!IsDpadKeyCodeUniq(currentKeyMapping, parameter)) {
        return false;
    }

    currentKeyMapping.keyCode = 0;
    currentKeyMapping.combinationKeys.clear();
    currentKeyMapping.SetSkillRangeRadiusToDefault();
    return true;
}

bool GameKeyMappingInfo::CheckCrosshairKey(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter)
{
    if (!IsStepValid(currentKeyMapping)) {
        return false;
    }
    if (++parameter.keyMappingNumber[MappingTypeEnum::CROSSHAIR_KEY_TO_TOUCH] > 1) {
        HILOGE("numbers of CROSSHAIR_KEY_TO_TOUCH have exceeded its limit[1]");
        return false;
    }
    if (parameter.uniqKeyCodeMap.find(currentKeyMapping.keyCode) != parameter.uniqKeyCodeMap.end()) {
        HILOGE("keycode has been used");
        return false;
    }
    parameter.uniqKeyCodeMap[currentKeyMapping.keyCode] = currentKeyMapping.mappingType;

    currentKeyMapping.SetDpadInfoToDefault();
    currentKeyMapping.combinationKeys.clear();
    currentKeyMapping.SetSkillRangeRadiusToDefault();
    return true;
}

bool GameKeyMappingInfo::CheckMouseLeftFire(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter)
{
    if (++parameter.keyMappingNumber[MappingTypeEnum::MOUSE_LEFT_FIRE_TO_TOUCH] > 1) {
        HILOGE("numbers of MOUSE_LEFT_FIRE_TO_TOUCH have exceeded its limit[1]");
        return false;
    }

    currentKeyMapping.keyCode = 0;
    currentKeyMapping.SetDpadInfoToDefault();
    currentKeyMapping.combinationKeys.clear();
    currentKeyMapping.SetSkillRangeRadiusToDefault();
    currentKeyMapping.SetStepToDefault();
    return true;
}

bool GameKeyMappingInfo::CheckMouseRightKeyClick(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter)
{
    if (++parameter.keyMappingNumber[MappingTypeEnum::MOUSE_RIGHT_KEY_CLICK_TO_TOUCH] > 1) {
        HILOGE("numbers of MOUSE_RIGHT_KEY_CLICK_TO_TOUCH have exceeded its limit[1]");
        return false;
    }
    if (parameter.uniqRightButtonMappingType != -1) {
        HILOGE("mouse right button has been used in MOUSE_RIGHT_KEY_WALKING_TO_TOUCH or MOUSE_OBSERVATION_TO_TOUCH");
        return false;
    }
    parameter.uniqRightButtonMappingType = MappingTypeEnum::MOUSE_RIGHT_KEY_CLICK_TO_TOUCH;
    currentKeyMapping.keyCode = 0;
    currentKeyMapping.SetDpadInfoToDefault();
    currentKeyMapping.combinationKeys.clear();
    currentKeyMapping.SetSkillRangeRadiusToDefault();
    currentKeyMapping.SetStepToDefault();
    return true;
}

bool GameKeyMappingInfo::CheckKeyMapping(std::vector<KeyToTouchMappingInfo> &KeyToTouchMappings)
{
    ParameterByCheck parameter;
    for (size_t i = 0; i < KeyToTouchMappings.size(); ++i) {
        MappingTypeEnum currentMappingType = KeyToTouchMappings[i].mappingType;
        if (checkKeyMappingHandlerMap.find(currentMappingType) != checkKeyMappingHandlerMap.end()) {
            if ((this->*checkKeyMappingHandlerMap[currentMappingType])(KeyToTouchMappings[i], parameter)) {
                continue;
            }
            return false;
        }
        HILOGE("MappingTypeEnum [%{public}d] is invalid", currentMappingType);
        return false;
    }
    return true;
}

bool GameKeyMappingInfo::IsDpadKeyCodeUniq(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter)
{
    if (parameter.uniqKeyCodeMap.find(currentKeyMapping.dpadKeyCodeEntity.up) != parameter.uniqKeyCodeMap.end()) {
        HILOGE("dpad up has been used.");
        return false;
    }
    parameter.uniqKeyCodeMap[currentKeyMapping.dpadKeyCodeEntity.up] = currentKeyMapping.mappingType;

    if (parameter.uniqKeyCodeMap.find(currentKeyMapping.dpadKeyCodeEntity.down) != parameter.uniqKeyCodeMap.end()) {
        HILOGE("dpad down has been used.");
        return false;
    }
    parameter.uniqKeyCodeMap[currentKeyMapping.dpadKeyCodeEntity.down] = currentKeyMapping.mappingType;

    if (parameter.uniqKeyCodeMap.find(currentKeyMapping.dpadKeyCodeEntity.left) != parameter.uniqKeyCodeMap.end()) {
        HILOGE("dpad left has been used.");
        return false;
    }
    parameter.uniqKeyCodeMap[currentKeyMapping.dpadKeyCodeEntity.left] = currentKeyMapping.mappingType;

    if (parameter.uniqKeyCodeMap.find(currentKeyMapping.dpadKeyCodeEntity.right) != parameter.uniqKeyCodeMap.end()) {
        HILOGE("dpad right has been used.");
        return false;
    }
    parameter.uniqKeyCodeMap[currentKeyMapping.dpadKeyCodeEntity.right] = currentKeyMapping.mappingType;
    return true;
}

bool GameKeyMappingInfo::IsStepValid(const KeyToTouchMappingInfo &currentKeyMapping)
{
    if (currentKeyMapping.xStep < 0 || currentKeyMapping.yStep < 0) {
        HILOGE("xStep or yStep cannot be smaller than 0");
        return false;
    }
    return true;
}

bool GameKeyMappingInfo::CheckKeyMappingForHoverTouchPad(std::vector<KeyToTouchMappingInfo> &KeyToTouchMappings)
{
    std::unordered_set<int32_t> uniqKeycodeSet;
    size_t keycodeNumber = 0;
    for (auto &currentKeyMapping: KeyToTouchMappings) {
        if (currentKeyMapping.mappingType != MappingTypeEnum::SINGE_KEY_TO_TOUCH) {
            HILOGE("HoverTouchPad only support SINGE_KEY_TO_TOUCH key mapping");
            return false;
        }
        if (++keycodeNumber > MAX_SINGLE_KEY_SIZE_FOR_HOVER_TOUCH_PAD) {
            HILOGE("numbers of SINGE_KEY_TO_TOUCH for HoverTouchPad have exceeded its limit[%{public}d]",
                   static_cast<int32_t>(MAX_SINGLE_KEY_SIZE_FOR_HOVER_TOUCH_PAD));
            return false;
        }
        if (uniqKeycodeSet.find(currentKeyMapping.keyCode) != uniqKeycodeSet.end()) {
            HILOGE("keycode has been used");
            return false;
        }
        uniqKeycodeSet.insert(currentKeyMapping.keyCode);

        currentKeyMapping.SetDpadInfoToDefault();
        currentKeyMapping.combinationKeys.clear();
        currentKeyMapping.SetSkillRangeRadiusToDefault();
        currentKeyMapping.SetStepToDefault();
    }
    return true;
}
}
}