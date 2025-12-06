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
#ifndef GAME_CONTROLLER_GAMECONTROLLER_CLIENT_MODEL_H
#define GAME_CONTROLLER_GAMECONTROLLER_CLIENT_MODEL_H

#include <string>
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <parcel.h>
#include <unordered_map>

namespace OHOS {
namespace GameController {
const size_t MAX_UNIQ_LENGTH = 50;
const int32_t MAX_SIZE = 10;
const size_t MAX_DEVICE_NAME_LENGTH = 256;

enum DeviceChangeType {
    REMOVE = 0,
    ADD = 1
};

/**
 * Interface Invoking Source
 */
enum ApiTypeEnum {
    ARK_TS = 0,
    CAPI = 0
};

/**
 * Device Type
 */
enum DeviceTypeEnum {
    UNKNOWN = 0,
    GAME_PAD = 1,
    HOVER_TOUCH_PAD = 2,
    GAME_KEY_BOARD = 3,
    GAME_MOUSE = 4,
};

/**
 * Input Source Enumeration Type
 */
enum InputSourceTypeEnum {
    KEYBOARD = 0,
    MOUSE = 1,
    TOUCHPAD = 2,
    TOUCHSCREEN = 3,
    JOYSTICK = 4,
    TRACKBALL = 5
};

/**
 * Axis classification of game pad
 */
enum GamePadAxisSourceTypeEnum {
    /*
     * directional pad
     */
    Dpad = 0,

    /**
     * Left and right thumbstick
     */
    LeftThumbstick = 1,
    RightThumbstick = 2,

    /*
     * Left and right trigger
     */
    LeftTriggerAxis = 3,
    RightTriggerAxis = 4
};

/**
 * Key classification of game pad
 */
enum GamePadButtonTypeEnum {
    LeftShoulder = 2307,
    RightShoulder = 2308,

    /*
     * Left and right trigger button
     */
    LeftTrigger = 2309,
    RightTrigger = 2310,

    /*
     * Left and right thumbstick button
     */
    LeftThumbstickButton = 2314,
    RightThumbstickButton = 2315,

    /*
     * Home key, corresponding to KEYCODE_BUTTON_ SELECT
     */
    ButtonHome = 2311,

    /*
     * Menu, corresponding to KEYCODE_BUTTON_START
     */
    ButtonMenu = 2312,

    /*
     * KEYCODE_BUTTON_MODE
     */
    ButtonOptions = 2313,

    ButtonA = 2301,
    ButtonB = 2302,
    ButtonC = 2303,
    ButtonX = 2304,
    ButtonY = 2305,

    /*
     * directional pad button
     */
    Dpad_UpButton = 2012,
    Dpad_DownButton = 2013,
    Dpad_LeftButton = 2014,
    Dpad_RightButton = 2015
};

/**
 * Identified Device Information
 */
struct IdentifiedDeviceInfo : public Parcelable {
    std::string name;
    int32_t product = 0;
    int32_t vendor = 0;
    DeviceTypeEnum deviceType = UNKNOWN;

    bool Marshalling(Parcel &parcel) const
    {
        if (!parcel.WriteString(name)) {
            return false;
        }
        if (!parcel.WriteInt32(product)) {
            return false;
        }
        if (!parcel.WriteInt32(vendor)) {
            return false;
        }
        if (!parcel.WriteInt32(deviceType)) {
            return false;
        }
        return true;
    }

    static IdentifiedDeviceInfo* Unmarshalling(Parcel &parcel)
    {
        IdentifiedDeviceInfo* ret = new(std::nothrow) IdentifiedDeviceInfo();
        if (ret == nullptr) {
            return nullptr;
        }
        if (!parcel.ReadString(ret->name)) {
            goto error;
        }
        if (!parcel.ReadInt32(ret->product)) {
            goto error;
        }

        if (!parcel.ReadInt32(ret->vendor)) {
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
     * Validates parameters of the SyncIdentifiedDeviceInfos interface.
     * @return true means valid
     */
    bool CheckParamValid() const
    {
        if (name.empty() || name.length() > MAX_DEVICE_NAME_LENGTH) {
            return false;
        }
        return true;
    }
};

struct BasicDeviceInfo {
    std::string uniq;
    std::string name;
    int32_t product = 0;
    int32_t vendor = 0;
    int32_t version = 0;
    std::string phys;
    DeviceTypeEnum deviceType = UNKNOWN;
};

/**
 * Device Information
 */
struct DeviceInfo : public Parcelable {
    std::string uniq;
    std::string name;
    int32_t product = 0;
    int32_t vendor = 0;
    int32_t version = 0;
    std::string phys;
    DeviceTypeEnum deviceType = UNKNOWN;
    std::unordered_set<int32_t> ids;
    std::unordered_set<std::string> names;
    std::unordered_set<InputSourceTypeEnum> sourceTypeSet;
    std::unordered_map<int32_t, std::unordered_set<InputSourceTypeEnum>> idSourceTypeMap;
    int64_t onlineTime = 0;
    int32_t status = 0; // 0-online;1-offline

    std::string vidPid;

    std::string anonymizationUniq;

    bool hasFullKeyBoard = false;

    bool Marshalling(Parcel &parcel) const
    {
        if (!parcel.WriteString(uniq)) {
            return false;
        }
        if (!parcel.WriteString(name)) {
            return false;
        }
        if (!parcel.WriteInt32(product)) {
            return false;
        }
        if (!parcel.WriteInt32(vendor)) {
            return false;
        }
        if (!parcel.WriteInt32(version)) {
            return false;
        }
        if (!parcel.WriteString(phys)) {
            return false;
        }
        if (!parcel.WriteInt32(deviceType)) {
            return false;
        }
        if (!parcel.WriteInt32(status)) {
            return false;
        }
        if (this->WriteArrayObject(parcel)) {
            return true;
        }
        return false;
    }

    bool WriteArrayObject(Parcel &parcel) const
    {
        if (!parcel.WriteInt32(ids.size())) {
            return false;
        }
        for (auto id: ids) {
            if (!parcel.WriteInt32(id)) {
                return false;
            }
        }
        if (!parcel.WriteInt32(names.size())) {
            return false;
        }
        for (auto name: names) {
            if (!parcel.WriteString(name)) {
                return false;
            }
        }
        if (!parcel.WriteInt32(sourceTypeSet.size())) {
            return false;
        }
        for (auto sourceType: sourceTypeSet) {
            if (!parcel.WriteInt32(sourceType)) {
                return false;
            }
        }
        return true;
    }

    static DeviceInfo* Unmarshalling(Parcel &parcel)
    {
        DeviceInfo* ret = new(std::nothrow) DeviceInfo();
        if (ret == nullptr) {
            return nullptr;
        }
        int32_t size;
        if (!Read(parcel, ret)) {
            goto error;
        }

        size = parcel.ReadInt32();
        if (size > MAX_SIZE) {
            goto error;
        }
        for (int i = 0; i < size; i++) {
            ret->ids.insert(parcel.ReadInt32());
        }
        size = parcel.ReadInt32();
        if (size > MAX_SIZE) {
            goto error;
        }
        for (int i = 0; i < size; i++) {
            ret->names.insert(parcel.ReadString());
        }

        size = parcel.ReadInt32();
        if (size > MAX_SIZE) {
            goto error;
        }
        for (int i = 0; i < size; i++) {
            ret->sourceTypeSet.insert(static_cast<InputSourceTypeEnum>(parcel.ReadInt32()));
        }
        return ret;
        error:
        delete ret;
        ret = nullptr;
        return nullptr;
    }

    static bool Read(Parcel &parcel, DeviceInfo* ret)
    {
        if (!parcel.ReadString(ret->uniq)) {
            return false;
        }
        if (!parcel.ReadString(ret->name)) {
            return false;
        }
        if (!parcel.ReadInt32(ret->product)) {
            return false;
        }

        if (!parcel.ReadInt32(ret->vendor)) {
            return false;
        }

        if (!parcel.ReadInt32(ret->version)) {
            return false;
        }

        if (!parcel.ReadString(ret->phys)) {
            return false;
        }
        int32_t deviceType;
        if (!parcel.ReadInt32(deviceType)) {
            return false;
        }
        ret->deviceType = static_cast<DeviceTypeEnum>(deviceType);
        if (!parcel.ReadInt32(ret->status)) {
            return false;
        }
        return true;
    }

    /**
     * Check whether uniq is empty.
     * @return The value true indicates that the value is null.
     */
    bool UniqIsEmpty()
    {
        return uniq.empty() || uniq == "null";
    }

    /**
     * Obtains the device information description.
     * @return Device information description
     */
    std::string GetDeviceInfoDesc()
    {
        std::string tmp = "uniq:" + anonymizationUniq;
        tmp.append(", name:" + name);
        tmp.append(", product:" + std::to_string(product));
        tmp.append(", vendor:" + std::to_string(vendor));
        tmp.append(", version:" + std::to_string(version));
        tmp.append(", deviceType:" + std::to_string(deviceType));
        tmp.append(", phys:" + phys);
        tmp.append(", ids:");
        for (auto id: ids) {
            tmp.append(std::to_string(id));
            tmp.append("|");
        }
        tmp.append(", sourceTypeSet:");
        for (auto &sourceType: sourceTypeSet) {
            tmp.append(std::to_string(sourceType));
            tmp.append("|");
        }
        tmp.append(", onlineTime:" + std::to_string(onlineTime));
        tmp.append(", hasFullKeyBoard:" + std::to_string(hasFullKeyBoard));
        return tmp;
    }

    /**
     * Validates parameters of the BroadcastDeviceInfo or BroadcastOpenTemplateConfig interface.
     * @return true means valid
     */
    bool CheckParamValid() const
    {
        if (uniq.empty() || uniq.length() > MAX_UNIQ_LENGTH) {
            return false;
        }

        if (name.empty() || name.length() > MAX_DEVICE_NAME_LENGTH) {
            return false;
        }
        return true;
    }

    std::string GetVidPid() const
    {
        return std::to_string(vendor) + "_" + std::to_string(product);
    }

    BasicDeviceInfo ConvertToBasicDeviceInfo() const
    {
        BasicDeviceInfo basicDeviceInfo;
        basicDeviceInfo.name = name;
        basicDeviceInfo.uniq = uniq;
        basicDeviceInfo.product = product;
        basicDeviceInfo.vendor = vendor;
        basicDeviceInfo.version = version;
        basicDeviceInfo.phys = phys;
        basicDeviceInfo.deviceType = deviceType;
        return basicDeviceInfo;
    }
};

/**
 * Device change event
 */
struct DeviceEvent {
    /**
     * Event type, which can be online or offline.
     */
    int32_t changeType = 0;

    /**
     * Device Information
     */
    DeviceInfo deviceInfo;
};

/**
 * Axis Event
 */
struct AxisEvent {
    int32_t id = 0;
    std::string uniq;
    GamePadAxisSourceTypeEnum axisSourceType;
    int64_t actionTime = 0;
};

struct GamePadAxisEvent : AxisEvent {
    double xValue = 0.0;
    double yValue = 0.0;
    double zValue = 0.0;
    double rzValue = 0.0;
    double brakeValue = 0.0;
    double gasValue = 0.0;
    double hatxValue = 0.0;
    double hatyValue = 0.0;
};

/**
 * Key Information
 */
struct KeyInfo {
    int32_t keyCode = 0;
    std::string keyCodeName;
    bool pressed = false;
    int64_t downTime = 0;
};

struct ButtonEvent {
    int32_t id = 0;
    std::string uniq;

    /*
     * down：0
     * up：1
     */
    int32_t keyAction = 0;
    int32_t keyCode = 0;
    std::string keyCodeName;
    std::vector<KeyInfo> keys;
    int64_t actionTime = 0;
};

struct GamePadButtonEvent : ButtonEvent {
};

/**
 * Base class for handle axis event callback.
 */
class GamePadAxisCallbackBase {
public:
    GamePadAxisCallbackBase() = default;

    virtual ~GamePadAxisCallbackBase() = default;

    /**
     * Executing an Event Callback Operation
     * @param event Called back event
     * @return Execution result
     */
    virtual int32_t OnAxisEventCallback(const GamePadAxisEvent &event);
};

/**
 * Base class for handle button event callback.
 */
class GamePadButtonCallbackBase {
public:
    GamePadButtonCallbackBase() = default;

    virtual ~GamePadButtonCallbackBase() = default;

    /**
     * Executing an Event Callback Operation
     * @param event Called back event
     * @return Execution result
     */
    virtual int32_t OnButtonEventCallback(const GamePadButtonEvent &event);
};

/**
 * Base class for handle device event callback.
 */
class DeviceEventCallbackBase {
public:
    DeviceEventCallbackBase() = default;

    virtual ~DeviceEventCallbackBase() = default;

    /**
     * Executing an Event Callback Operation
     * @param event Called back event
     * @return Execution result
     */
    virtual int32_t OnDeviceEventCallback(const DeviceEvent &event);
};
}
}
#endif //GAME_CONTROLLER_GAMECONTROLLER_CLIENT_MODEL_H