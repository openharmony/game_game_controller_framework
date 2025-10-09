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
    void SetSupportKeyMapping(bool isSupportKeyMapping);

    bool DispatchKeyEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent);

    bool DispatchPointerEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    void UpdateTemplateConfig(const DeviceTypeEnum &deviceType, const std::vector<KeyToTouchMappingInfo> &mappingInfos);

    void UpdateWindowInfo(const WindowInfoEntity &windowInfoEntity);

    void EnableKeyMapping(bool isEnable);

private:

    bool IsCanEnableKeyMapping();

    void HandleKeyEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent, const DeviceTypeEnum &deviceType,
                        const DeviceInfo &deviceInfo);

    void HandlePointerEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent, const DeviceTypeEnum &deviceType);

    /**
     * Get mapping info
     * @param context context
     * @param keyEvent keyEvent
     * @param deviceInfo deviceInfo
     * @param keyToTouchMappingInfo out param
     * @return true means get mapping info success
     */
    bool GetMappingInfoByKeyCode(const std::shared_ptr<InputToTouchContext> &context,
                                 const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                 const DeviceInfo &deviceInfo,
                                 KeyToTouchMappingInfo &keyToTouchMappingInfo);

    bool GetMappingInfoByKeyCodeWhenKeyDown(const std::shared_ptr<InputToTouchContext> &context,
                                            const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                            const DeviceInfo &deviceInfo,
                                            KeyToTouchMappingInfo &keyToTouchMappingInfo);

    bool GetMappingInfoByKeyCodeWhenKeyUp(const std::shared_ptr<InputToTouchContext> &context,
                                          const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                          KeyToTouchMappingInfo &keyToTouchMappingInfo);

    bool GetMappingInfoByKeyCodeFromCombinationKey(const std::shared_ptr<InputToTouchContext> &context,
                                                   const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                   const DeviceInfo &deviceInfo,
                                                   KeyToTouchMappingInfo &keyToTouchMappingInfo);

    void HandleTemplateConfig(const DeviceTypeEnum &deviceType, const std::vector<KeyToTouchMappingInfo> &mappingInfos);

    void HandleWindowInfo(const WindowInfoEntity &windowInfoEntity);

    void UpdateContextWindowInfo(const std::shared_ptr<InputToTouchContext> &context);

    void InitGcKeyboardContext(const std::vector<KeyToTouchMappingInfo> &mappingInfos);

    void InitHoverTouchPadContext(const std::vector<KeyToTouchMappingInfo> &mappingInfos);

    void ReleaseContext(const std::shared_ptr<InputToTouchContext> &inputToTouchContext);

    void ResetMonitor();

    void ResetAllMonitorKeysAndMouseMonitor(const std::shared_ptr<InputToTouchContext> &context);

    void AddToMonitorKeys(const DeviceTypeEnum &deviceType,
                          const std::unordered_map<int32_t, KeyToTouchMappingInfo> &keyMap);

    void AddToMonitorKeys(const DeviceTypeEnum &deviceType,
                          const int32_t &keyCode);

    bool IsHandleMouseMove(std::shared_ptr<InputToTouchContext> &context,
                           const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    bool IsHandleMouseRightButtonEvent(std::shared_ptr<InputToTouchContext> &context,
                                       const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    bool IsHandleMouseLeftButtonEvent(std::shared_ptr<InputToTouchContext> &context,
                                      const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    void ExecuteHandle(std::shared_ptr<InputToTouchContext> &context,
                       const KeyToTouchMappingInfo &mappingInfo,
                       const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                       const DeviceInfo &deviceInfo);

    void ExecuteHandle(std::shared_ptr<InputToTouchContext> &context,
                       const KeyToTouchMappingInfo &mappingInfo,
                       const std::shared_ptr<MMI::PointerEvent> &pointerEvent);

    void HandleEnableKeyMapping(bool isEnable);

    void ResetContext(std::shared_ptr<InputToTouchContext> &context);

private:
    bool isSupportKeyMapping_{false};
    ffrt::mutex checkMutex_;
    std::unique_ptr<ffrt::queue> handleQueue_{nullptr};
    std::unordered_map<MappingTypeEnum, std::shared_ptr<BaseKeyToTouchHandler>> mappingHandler_;
    std::shared_ptr<InputToTouchContext> gcKeyboardContext_{nullptr};
    std::shared_ptr<InputToTouchContext> hoverTouchPadContext_{nullptr};
    std::unordered_map<int32_t, std::unordered_set<DeviceTypeEnum>> allMonitorKeys_;
    bool isMonitorMouse_ = false;
    std::unordered_set<int32_t> allMonitorMousePointerActions_;
    WindowInfoEntity windowInfoEntity_;
    bool isEnableKeyMapping_{true};
};
}
}

#endif //GAME_CONTROLLER_FRAMEWORK_KEY_TO_TOUCH_MANAGER_H
