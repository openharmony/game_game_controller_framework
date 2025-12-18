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

#include <display.h>
#include <input_manager.h>
#include "key_mapping_handle.h"
#include "gamecontroller_log.h"
#include "multi_modal_input_mgt_service.h"
#include "key_mapping_service.h"

using namespace OHOS::MMI;
namespace OHOS {
namespace GameController {
namespace {
const int32_t KEYCODE_OPEN_TEMP_FOR_HOVER_TOUCH_CONTROLLER = 3107;
const int32_t KEYS_REQUIRED_FOR_TEMPLATE_OPEN = 3;
}

KeyMappingHandle::KeyMappingHandle()
{
    isOpenTemplateValidHandlerMap_ = {
        {DeviceTypeEnum::HOVER_TOUCH_PAD, &KeyMappingHandle::OpenTemplateByHoverTouchPad},
        {DeviceTypeEnum::GAME_KEY_BOARD,  &KeyMappingHandle::OpenTemplateByKeyBoard}
    };
}

KeyMappingHandle::~KeyMappingHandle()
{
}

bool KeyMappingHandle::IsNotifyOpenTemplateConfigPage(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    if (!isSupportKeyMapping_) {
        return false;
    }

    // Only the key up and down event needs to be processed.
    if (keyEvent->GetKeyAction() != KeyEvent::KEY_ACTION_DOWN) {
        return false;
    }

    DeviceInfo deviceInfo = DelayedSingleton<MultiModalInputMgtService>::GetInstance()->GetDeviceInfo(
        keyEvent->GetDeviceId());
    if (deviceInfo.UniqIsEmpty()) {
        return false;
    }

    return IsNotifyOpenTemplateConfigPage(keyEvent, deviceInfo);
}

void KeyMappingHandle::SetSupportKeyMapping(bool isSupportKeyMapping)
{
    isSupportKeyMapping_ = isSupportKeyMapping;
}

bool KeyMappingHandle::IsNotifyOpenTemplateConfigPage(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                      const DeviceInfo &deviceInfo)
{
    DeviceTypeEnum deviceType = deviceInfo.deviceType;

    if (isOpenTemplateValidHandlerMap_.find(deviceType) != isOpenTemplateValidHandlerMap_.end()) {
        return (this->*isOpenTemplateValidHandlerMap_[deviceType])(keyEvent, deviceInfo);
    } else {
        if (IsFullKeyboard(deviceInfo)) {
            DeviceInfo temp;
            temp.name = deviceInfo.name;
            temp.deviceType = GAME_KEY_BOARD;
            temp.uniq = deviceInfo.uniq;
            temp.onlineTime = deviceInfo.onlineTime;
            return (this->*isOpenTemplateValidHandlerMap_[GAME_KEY_BOARD])(keyEvent, temp);
        }
        return false;
    }
}

bool KeyMappingHandle::OpenTemplateByHoverTouchPad(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                   const DeviceInfo &deviceInfo)
{
    if (keyEvent->GetKeyCode() == KEYCODE_OPEN_TEMP_FOR_HOVER_TOUCH_CONTROLLER) {
        HILOGI("Open template config page by HoverTouchPad.");
        DelayedSingleton<KeyMappingService>::GetInstance()->BroadcastOpenTemplateConfig(deviceInfo);
        return true;
    } else {
        return false;
    }
}

bool KeyMappingHandle::OpenTemplateByKeyBoard(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                              const DeviceInfo &deviceInfo)
{
    std::unordered_map<int32_t, MMI::KeyEvent::KeyItem> currentItemsMap;

    std::vector<MMI::KeyEvent::KeyItem> keyItems = keyEvent->GetKeyItems();
    // Filter out the invalid keyItem.
    for (const auto &keyItem: keyItems) {
        if (!keyItem.IsPressed() || keyItem.GetDeviceId() != keyEvent->GetDeviceId() ||
            keyItem.GetDownTime() < deviceInfo.onlineTime) {
            continue;
        }
        currentItemsMap.insert({keyItem.GetKeyCode(), keyItem});
    }

    // Return true only when CTRL+SHIFT+I are pressed.
    if (currentItemsMap.size() == KEYS_REQUIRED_FOR_TEMPLATE_OPEN &&
        (currentItemsMap.find(MMI::KeyEvent::KEYCODE_CTRL_LEFT) != currentItemsMap.end() ||
            currentItemsMap.find(MMI::KeyEvent::KEYCODE_CTRL_RIGHT) != currentItemsMap.end()) &&
        (currentItemsMap.find(MMI::KeyEvent::KEYCODE_SHIFT_LEFT) != currentItemsMap.end() ||
            currentItemsMap.find(MMI::KeyEvent::KEYCODE_SHIFT_RIGHT) != currentItemsMap.end()) &&
        currentItemsMap.find(MMI::KeyEvent::KEYCODE_I) != currentItemsMap.end()) {
        HILOGI("Open template config page by KeyBoard is valid.");
        DelayedSingleton<KeyMappingService>::GetInstance()->BroadcastOpenTemplateConfig(deviceInfo);
        return true;
    }
    return false;
}

bool KeyMappingHandle::IsFullKeyboard(const DeviceInfo &deviceInfo)
{
    return deviceInfo.deviceType == DeviceTypeEnum::UNKNOWN && deviceInfo.hasFullKeyBoard;
}
}
}