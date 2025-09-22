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
#ifndef GAME_CONTROLLER_GAMECONTROLLER_KEYMAPPING_MODEL_H
#define GAME_CONTROLLER_GAMECONTROLLER_KEYMAPPING_MODEL_H

#include <string>
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <parcel.h>
#include <memory>
#include "gamecontroller_client_model.h"

namespace OHOS {
namespace GameController {
const size_t MAX_COMBINATION_KEYS = 2;
const size_t COMBINATION_FIRST_KEYCODE_IDX = 0;
const size_t COMBINATION_LAST_KEYCODE_IDX = 1;
const int32_t MAX_KEY_MAPPING_SIZE = 100;
const size_t MAX_BUNDLE_NAME_LENGTH = 256;
const size_t MAX_VERSION_LENGTH = 50;
const size_t SUM_OF_MAPPING_TYPE = 11;

struct ParameterByCheck {
    std::vector<size_t> keyMappingNumber = std::vector<size_t>(SUM_OF_MAPPING_TYPE, 0);
    std::unordered_map<int32_t, int32_t> uniqKeyCodeMap;
    std::unordered_map<int32_t, int32_t> combinationKeysMap;
    int32_t uniqRightButtonMappingType = -1;
};

enum MappingTypeEnum {
    // Single-button switch to touch clicking
    SINGE_KEY_TO_TOUCH = 0,

    // Combining-button switch to touch clicking
    COMBINATION_KEY_TO_TOUCH = 1,

    // DirectionPad of keyboard switch to touch clicking and moving
    DPAD_KEY_TO_TOUCH = 2,

    // Use the right-button of mouse to walk.
    MOUSE_RIGHT_KEY_WALKING_TO_TOUCH = 3,

    // Skill key
    SKILL_KEY_TO_TOUCH = 4,

    // Observation Perspective Key
    OBSERVATION_KEY_TO_TOUCH = 5,

    // Use the right-button of mouse for observation
    MOUSE_OBSERVATION_TO_TOUCH = 6,

    // Use the keyboard for observation
    KEY_BOARD_OBSERVATION_TO_TOUCH = 7,

    // Crosshair key for shooting games
    CROSSHAIR_KEY_TO_TOUCH = 8,

    // Use the left-button of mouse to shoot for shooting games
    MOUSE_LEFT_FIRE_TO_TOUCH = 9,

    // Right-button of mouse switch to touch clicking
    MOUSE_RIGHT_KEY_CLICK_TO_TOUCH = 10
};

/**
 * the information of the direction pad
 */
struct DpadKeyCodeEntity : public Parcelable {
    int32_t up = 0;
    int32_t down = 0;
    int32_t left = 0;
    int32_t right = 0;

    bool Marshalling(Parcel &parcel) const
    {
        if (!parcel.WriteInt32(up)) {
            return false;
        }
        if (!parcel.WriteInt32(down)) {
            return false;
        }
        if (!parcel.WriteInt32(left)) {
            return false;
        }
        if (!parcel.WriteInt32(right)) {
            return false;
        }
        return true;
    }

    static DpadKeyCodeEntity* Unmarshalling(Parcel &parcel)
    {
        DpadKeyCodeEntity* ret = new(std::nothrow) DpadKeyCodeEntity();
        if (ret == nullptr) {
            return nullptr;
        }

        if (!parcel.ReadInt32(ret->up)) {
            goto error;
        }
        if (!parcel.ReadInt32(ret->down)) {
            goto error;
        }
        if (!parcel.ReadInt32(ret->left)) {
            goto error;
        }
        if (!parcel.ReadInt32(ret->right)) {
            goto error;
        }
        return ret;
        error:
        delete ret;
        ret = nullptr;
        return nullptr;
    }

    bool Contains(int32_t keyCode)
    {
        return up == keyCode || down == keyCode || left == keyCode || right == keyCode;
    }
};

/**
 * Key-to-Touch Mapping
 */
struct KeyToTouchMappingInfo : public Parcelable {
    int32_t keyCode = 0;

    int32_t xValue = 0;

    int32_t yValue = 0;

    MappingTypeEnum mappingType = SINGE_KEY_TO_TOUCH;

    DpadKeyCodeEntity dpadKeyCodeEntity;

    std::vector<int32_t> combinationKeys;

    int32_t radius = 0;

    int32_t skillRange = 0;

    int32_t xStep = 0;

    int32_t yStep = 0;

    bool Marshalling(Parcel &parcel) const
    {
        if (!parcel.WriteInt32(keyCode)) {
            return false;
        }
        if (!parcel.WriteInt32(xValue)) {
            return false;
        }
        if (!parcel.WriteInt32(yValue)) {
            return false;
        }
        if (!parcel.WriteInt32(mappingType)) {
            return false;
        }
        if (!parcel.WriteInt32(radius)) {
            return false;
        }
        if (!parcel.WriteInt32(skillRange)) {
            return false;
        }
        if (!parcel.WriteInt32(xStep)) {
            return false;
        }
        if (!parcel.WriteInt32(yStep)) {
            return false;
        }
        if (!parcel.WriteParcelable(&dpadKeyCodeEntity)) {
            return false;
        }
        return WriteCombinationKeys(parcel);
    }

    bool WriteCombinationKeys(Parcel &parcel) const
    {
        if (combinationKeys.size() > MAX_COMBINATION_KEYS) {
            return false;
        }
        if (!parcel.WriteInt32(combinationKeys.size())) {
            return false;
        }
        for (int32_t key: combinationKeys) {
            if (!parcel.WriteInt32(key)) {
                return false;
            }
        }
        return true;
    }

    static KeyToTouchMappingInfo* Unmarshalling(Parcel &parcel)
    {
        KeyToTouchMappingInfo* ret = new(std::nothrow) KeyToTouchMappingInfo();
        if (ret == nullptr) {
            return nullptr;
        }

        if (!parcel.ReadInt32(ret->keyCode)) {
            goto error;
        }
        if (!parcel.ReadInt32(ret->xValue)) {
            goto error;
        }
        if (!parcel.ReadInt32(ret->yValue)) {
            goto error;
        }

        if (!ReadMappingType(parcel, ret)) {
            goto error;
        }

        if (!parcel.ReadInt32(ret->radius)) {
            goto error;
        }
        if (!parcel.ReadInt32(ret->skillRange)) {
            goto error;
        }
        if (!parcel.ReadInt32(ret->xStep)) {
            goto error;
        }
        if (!parcel.ReadInt32(ret->yStep)) {
            goto error;
        }
        if (!ReadDpadInfo(parcel, ret)) {
            goto error;
        }
        if (!ReadCombinationKeys(parcel, ret)) {
            goto error;
        }
        return ret;
        error:
        delete ret;
        ret = nullptr;
        return nullptr;
    }

    static bool ReadMappingType(Parcel &parcel, KeyToTouchMappingInfo* ret)
    {
        int32_t mappingType;
        if (!parcel.ReadInt32(mappingType)) {
            return false;
        }

        ret->mappingType = static_cast<MappingTypeEnum>(mappingType);
        return true;
    }

    static bool ReadDpadInfo(Parcel &parcel, KeyToTouchMappingInfo* ret)
    {
        std::unique_ptr<DpadKeyCodeEntity> dpadInfo(parcel.ReadParcelable<DpadKeyCodeEntity>());
        if (dpadInfo) {
            ret->dpadKeyCodeEntity = *dpadInfo;
            return true;
        }
        return false;
    }

    static bool ReadCombinationKeys(Parcel &parcel, KeyToTouchMappingInfo* ret)
    {
        int32_t size = parcel.ReadInt32();
        if (size > static_cast<int32_t>(MAX_COMBINATION_KEYS)) {
            return false;
        }
        std::vector<int32_t> combinationKeys;
        for (int32_t idx = 0; idx < size; idx++) {
            combinationKeys.push_back(parcel.ReadInt32());
        }
        ret->combinationKeys = combinationKeys;
        return true;
    }

    /**
    * Obtains key-to-touch mapping information description.
    * @return KeyToTouchMappingInfo description
    */
    std::string GetKeyToTouchMappingInfoDesc() const
    {
        std::string tmp = "mappingType:" + std::to_string(mappingType);
        tmp.append(", keyCode:" + std::to_string(keyCode));
        tmp.append(", xValue:" + std::to_string(xValue));
        tmp.append(", yValue:" + std::to_string(yValue));
        tmp.append(", radius:" + std::to_string(radius));
        tmp.append(", skillRange:" + std::to_string(skillRange));
        tmp.append(", xStep:" + std::to_string(xStep));
        tmp.append(", yStep:" + std::to_string(yStep));
        tmp.append(", combinationKeys:");
        for (auto combinationKey: combinationKeys) {
            tmp.append(std::to_string(combinationKey) + "|");
        }
        tmp.append(", dpadKeyCode:" + std::to_string(dpadKeyCodeEntity.up));
        tmp.append("|" + std::to_string(dpadKeyCodeEntity.down));
        tmp.append("|" + std::to_string(dpadKeyCodeEntity.left));
        tmp.append("|" + std::to_string(dpadKeyCodeEntity.right));
        return tmp;
    }

    void SetDpadInfoToDefault()
    {
        dpadKeyCodeEntity.up = 0;
        dpadKeyCodeEntity.down = 0;
        dpadKeyCodeEntity.left = 0;
        dpadKeyCodeEntity.right = 0;
    }

    void SetSkillRangeRadiusToDefault()
    {
        skillRange = 0;
        radius = 0;
    }

    void SetStepToDefault()
    {
        xStep = 0;
        yStep = 0;
    }
};

/**
 * Parameters for getting key mapping
 */
struct GetGameKeyMappingInfoParam : public Parcelable {
    std::string bundleName;

    std::string uniq;

    DeviceTypeEnum deviceType;

    bool Marshalling(Parcel &parcel) const
    {
        if (!parcel.WriteString(bundleName)) {
            return false;
        }
        if (!parcel.WriteInt32(deviceType)) {
            return false;
        }
        return true;
    }

    static GetGameKeyMappingInfoParam* Unmarshalling(Parcel &parcel)
    {
        GetGameKeyMappingInfoParam* ret = new(std::nothrow) GetGameKeyMappingInfoParam();
        if (ret == nullptr) {
            return nullptr;
        }

        if (!parcel.ReadString(ret->bundleName)) {
            goto error;
        }
        int32_t deviceType;
        if (!parcel.ReadInt32(deviceType)) {
            goto error;
        }
        ret->deviceType = static_cast<DeviceTypeEnum>(deviceType);
        return ret;
        error:
        delete ret;
        ret = nullptr;
        return nullptr;
    }

    /**
     * Validates parameters of the GetGameKeyMappingConfig interface.
     * @return true means valid
     */
    bool CheckParamValid() const
    {
        if (bundleName.empty() || bundleName.length() > MAX_BUNDLE_NAME_LENGTH) {
            return false;
        }
        return true;
    }
};

struct GameInfo : public Parcelable {
    std::string bundleName;

    // The version of game
    std::string version;

    bool isSupportKeyMapping;

    bool Marshalling(Parcel &parcel) const
    {
        if (!parcel.WriteString(bundleName)) {
            return false;
        }
        if (!parcel.WriteString(version)) {
            return false;
        }
        if (!parcel.WriteBool(isSupportKeyMapping)) {
            return false;
        }
        return true;
    }

    static GameInfo* Unmarshalling(Parcel &parcel)
    {
        GameInfo* ret = new(std::nothrow) GameInfo();
        if (ret == nullptr) {
            return nullptr;
        }

        if (!parcel.ReadString(ret->bundleName)) {
            goto error;
        }
        if (!parcel.ReadString(ret->version)) {
            goto error;
        }

        if (!parcel.ReadBool(ret->isSupportKeyMapping)) {
            goto error;
        }
        return ret;
        error:
        delete ret;
        ret = nullptr;
        return nullptr;
    }

    /**
     * Validates parameters of the SyncSupportKeyMappingGames interface.
     * @return true means valid
     */
    bool CheckParamValid() const
    {
        if (bundleName.empty() || bundleName.length() > MAX_BUNDLE_NAME_LENGTH) {
            return false;
        }
        if (version.empty() || version.length() > MAX_VERSION_LENGTH) {
            return false;
        }
        return true;
    }
};

/**
 * Key mapping information of the game
 * if call SetCustomGameKeyMappingConfig interface,
 *    need to set bundleName and deviceType and customKeyToTouchMapping.
 * if call SetDefaultGameKeyMappingConfig interface,
 *    need to set bundleName and deviceType and defaultKeyToTouchMapping.
 */
struct GameKeyMappingInfo : public Parcelable {
    std::string bundleName;

    std::string uniq;

    /**
     * when call SetCustomGameKeyMappingConfig or SetDefaultGameKeyMappingConfig interface，
     * set the parameter to true means delete the config
     */
    bool isOprDelete;

    DeviceTypeEnum deviceType;

    std::vector<KeyToTouchMappingInfo> customKeyToTouchMappings;

    std::vector<KeyToTouchMappingInfo> defaultKeyToTouchMappings;

    bool Marshalling(Parcel &parcel) const
    {
        if (!parcel.WriteString(bundleName)) {
            return false;
        }

        if (!parcel.WriteBool(isOprDelete)) {
            return false;
        }

        if (!parcel.WriteInt32(deviceType)) {
            return false;
        }

        if (!parcel.WriteInt32(customKeyToTouchMappings.size())) {
            return false;
        }
        for (KeyToTouchMappingInfo keyMapping: customKeyToTouchMappings) {
            if (!parcel.WriteParcelable(&keyMapping)) {
                return false;
            }
        }
        if (!parcel.WriteInt32(defaultKeyToTouchMappings.size())) {
            return false;
        }
        for (KeyToTouchMappingInfo keyMapping: defaultKeyToTouchMappings) {
            if (!parcel.WriteParcelable(&keyMapping)) {
                return false;
            }
        }
        return true;
    }

    static GameKeyMappingInfo* Unmarshalling(Parcel &parcel)
    {
        GameKeyMappingInfo* ret = new(std::nothrow) GameKeyMappingInfo();
        if (ret == nullptr) {
            return nullptr;
        }
        if (!parcel.ReadString(ret->bundleName)) {
            goto error;
        }

        if (!parcel.ReadBool(ret->isOprDelete)) {
            goto error;
        }
        int32_t deviceType;
        if (!parcel.ReadInt32(deviceType)) {
            goto error;
        }
        ret->deviceType = static_cast<DeviceTypeEnum>(deviceType);
        if (!ReadCustomKeyMapping(parcel, ret)) {
            goto error;
        }
        if (!ReadDefaultKeyMapping(parcel, ret)) {
            goto error;
        }
        return ret;
        error:
        delete ret;
        ret = nullptr;
        return nullptr;
    }

    static bool ReadCustomKeyMapping(Parcel &parcel, GameKeyMappingInfo* ret)
    {
        int32_t size = parcel.ReadInt32();
        if (size > MAX_KEY_MAPPING_SIZE) {
            return false;
        }
        for (int i = 0; i < size; i++) {
            std::unique_ptr<KeyToTouchMappingInfo> keyMapping(parcel.ReadParcelable<KeyToTouchMappingInfo>());
            if (!keyMapping) {
                return false;
            }

            ret->customKeyToTouchMappings.push_back(std::move(*keyMapping));
        }
        return true;
    }

    static bool ReadDefaultKeyMapping(Parcel &parcel, GameKeyMappingInfo* ret)
    {
        int32_t size = parcel.ReadInt32();
        if (size > MAX_KEY_MAPPING_SIZE) {
            return false;
        }
        for (int i = 0; i < size; i++) {
            std::unique_ptr<KeyToTouchMappingInfo> keyMapping(parcel.ReadParcelable<KeyToTouchMappingInfo>());
            if (!keyMapping) {
                return false;
            }

            ret->defaultKeyToTouchMappings.push_back(std::move(*keyMapping));
        }
        return true;
    }

    /**
     * Validates parameters of the SetCustomGameKeyMappingConfig interface.
     * @return true means valid
     */
    bool CheckParamValidForSetCustom()
    {
        return CheckParamValidForSetDefault();
    };

    /**
     * Validates parameters of the SetDefaultGameKeyMappingConfig interface.
     * @return true means valid
     */
    bool CheckParamValidForSetDefault()
    {
        if (bundleName.empty() || bundleName.length() > MAX_BUNDLE_NAME_LENGTH) {
            return false;
        }
        if (deviceType == UNKNOWN && !isOprDelete) {
            return false;
        }
        if (customKeyToTouchMappings.size() > static_cast<size_t>(MAX_KEY_MAPPING_SIZE)) {
            return false;
        }
        if (defaultKeyToTouchMappings.size() > static_cast<size_t>(MAX_KEY_MAPPING_SIZE)) {
            return false;
        }
        if (deviceType == DeviceTypeEnum::HOVER_TOUCH_PAD) {
            return CheckKeyMappingForHoverTouchPad(defaultKeyToTouchMappings) &&
                CheckKeyMappingForHoverTouchPad(customKeyToTouchMappings);
        }
        return CheckKeyMapping(defaultKeyToTouchMappings) && CheckKeyMapping(customKeyToTouchMappings);
    }

    /**
     * Validates a GameKeyMappingConfig.
     * @return true means valid
     */
    bool CheckKeyMapping(std::vector<KeyToTouchMappingInfo> &KeyToTouchMappings);

    bool CheckSingleKey(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

    bool CheckCombinationKey(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

    bool CheckDpadKey(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

    bool CheckMouseRightKeyWalking(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

    bool CheckSkillKey(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

    bool CheckObservationKey(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

    bool CheckMouseObservation(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

    bool CheckKeyBoardObservation(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

    bool CheckCrosshairKey(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

    bool CheckMouseLeftFire(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

    bool CheckMouseRightKeyClick(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

    bool IsDpadKeyCodeUniq(KeyToTouchMappingInfo &currentKeyMapping, ParameterByCheck &parameter);

    bool CheckKeyMappingForHoverTouchPad(std::vector<KeyToTouchMappingInfo> &KeyToTouchMappings);

    bool IsStepValid(const KeyToTouchMappingInfo &currentKeyMapping);

    /**
     * Indicates whether to delete the default configuration based on the bundle name.
     * @return true means delete the default configuration based on the bundle name.
     */
    bool IsDelByBundleNameWhenSetDefault() const
    {
        return isOprDelete && deviceType == UNKNOWN;
    }

    /**
     * Indicates whether to delete the default configuration based on the bundle name.
     * @return true means delete the default configuration based on the bundle name.
     */
    bool IsDelByBundleNameWhenSetCustom() const
    {
        return IsDelByBundleNameWhenSetDefault();
    }
};
}
}
#endif //GAME_CONTROLLER_GAMECONTROLLER_KEYMAPPING_MODEL_H