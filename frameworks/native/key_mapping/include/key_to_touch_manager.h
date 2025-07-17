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

#ifndef GAME_CONTROLLER_FRAMEWORK_KEY_TO_TOUCH_MANAGER_H
#define GAME_CONTROLLER_FRAMEWORK_KEY_TO_TOUCH_MANAGER_H

#include <singleton.h>
#include <unordered_map>
#include "key_to_touch_handler.h"
#include "gamecontroller_keymapping_model.h"
#include "ffrt.h"

namespace OHOS {
namespace GameController {
class KeyToTouchManager : public DelayedSingleton<KeyToTouchManager> {
DECLARE_DELAYED_SINGLETON(KeyToTouchManager)

public:
    bool DispatchKeyEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent);

    bool DispatchPointerEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    void UpdateTemplateConfig(const DeviceInfo &deviceInfo, const std::vector<KeyToTouchMappingInfo> &mappingInfos);

    void UpdateWindowInfo(const WindowInfoEntity &windowInfoEntity);

private:
    bool HandleKeyEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent, const DeviceInfo &deviceInfo);

    bool HandlePointerEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent, const DeviceInfo &deviceInfo);

    void HandleTemplateConfig(const DeviceInfo &deviceInfo, const std::vector<KeyToTouchMappingInfo> &mappingInfos);

    void HandleWindowInfo(const WindowInfoEntity &windowInfoEntity);

private:
    std::mutex checkMutex_;
    std::unique_ptr<ffrt::queue> handleQueue_{nullptr};

    std::unordered_map<MappingTypeEnum, BaseKeyToTouchHandler> mappingHandler_;
    InputToTouchContext gcKeyboardContext_;
    InputToTouchContext hoverTouchPadContext_;
    std::unordered_map<int32_t, std::unordered_set<DeviceTypeEnum>> allMonitorKeys_;
    bool isMonitorMouse_ = false;
    std::unordered_set<int32_t> allMonitorMousePointerActions_;
    WindowInfoEntity windowInfoEntity_;
};
}
}

#endif //GAME_CONTROLLER_FRAMEWORK_KEY_TO_TOUCH_MANAGER_H
