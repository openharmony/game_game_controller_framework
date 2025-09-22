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

#ifndef GAME_CONTROLLER_FRAMEWORK_KEY_MAPPING_HANDLE_H
#define GAME_CONTROLLER_FRAMEWORK_KEY_MAPPING_HANDLE_H

#include <singleton.h>
#include <window.h>
#include "window_input_intercept_consumer.h"
#include "gamecontroller_client_model.h"
#include "gamecontroller_keymapping_model.h"

namespace OHOS {
namespace GameController {
class KeyMappingHandle : public DelayedSingleton<KeyMappingHandle> {
DECLARE_DELAYED_SINGLETON(KeyMappingHandle)

public:
    typedef bool(KeyMappingHandle::*IsOpenTemplateValidHandler)(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                                                const DeviceInfo &deviceInfo);

    void SetSupportKeyMapping(bool isSupportKeyMapping);

    /**
     * Indicates whether to perform key-to-touch control.
     * @param keyEvent Key Event
     * @return true means perform key-to-touch control.
     */
    bool IsNotifyOpenTemplateConfigPage(const std::shared_ptr<MMI::KeyEvent> &keyEvent);

    void SetIsPC(bool isPCDevice);

private:
    /**
     * Send Notification of Opening Template Configuration Page
     * @param keyEvent Key Event
     * @param deviceInfo device information
     * @return
     */
    bool IsNotifyOpenTemplateConfigPage(const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                        const DeviceInfo &deviceInfo);

    bool OpenTemplateByHoverTouchPad(const std::shared_ptr<MMI::KeyEvent> &keyEvent, const DeviceInfo &deviceInfo);

    bool OpenTemplateByKeyBoard(const std::shared_ptr<MMI::KeyEvent> &keyEvent, const DeviceInfo &deviceInfo);

    bool IsFullKeyboard(const DeviceInfo &deviceInfo);

private:

    bool isSupportKeyMapping_{false};
    bool isPc_{false};
    std::unordered_map<DeviceTypeEnum, IsOpenTemplateValidHandler> isOpenTemplateValidHandlerMap_;
};
}
}
#endif //GAME_CONTROLLER_FRAMEWORK_KEY_MAPPING_HANDLE_H
