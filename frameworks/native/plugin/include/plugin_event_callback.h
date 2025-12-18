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

#ifndef GAME_CONTROLLER_PLUGIN_EVENT_CALLBACK_H
#define GAME_CONTROLLER_PLUGIN_EVENT_CALLBACK_H

#include <string>
#include <memory>
#include <key_event.h>
#include <pointer_event.h>
#include "gamecontroller_client_model.h"

namespace OHOS {
namespace GameController {
/**
 * Callback for device events
 */
class DeviceCallback {
public:
    /**
     * Callback for device status changes
     * @param bundleName The currently notified game bundleName
     * @param deviceInfo DeviceInfo
     */
    virtual void OnDeviceStatusChanged(const std::string &bundleName, const DeviceInfo &deviceInfo)
    {
    };
};

/**
 * Callback for performing window operations
 */
class WindowOprCallback {
public:
    /**
     * Send key event to the current game
     * @param bundleName The bundleName of the current game
     * @param keyEvent key event
     * @param isSimulate True represents a simulated event
     * @return when it's pluginmode and the result is false,
     * send the event by WindowInputInterceptClient::SendInputEvent
     */
    virtual bool SendInputEvent(const std::string &bundleName, const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                bool isSimulate)
    {
        return true;
    };

    /**
     * Send pointer event to the game
     * @param bundleName The bundleName of the current game
     * @param pointerEvent pointer event
     * @param isSimulate True represents a simulated event
     * @return when it's pluginmode and the result is false,
     * send the event by WindowInputInterceptClient::SendInputEvent
     */
    virtual bool SendInputEvent(const std::string &bundleName, const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                bool isSimulate)
    {
        return true;
    };

    /**
     * Restore the gesture back state of the current game
     * @param The bundleName of the current game
     */
    virtual void RecoverGestureBackStatus(const std::string &bundleName)
    {
    };

    /**
     * Disable the gesture back
     * @param bundleName The bundleName of the current game
     */
    virtual void DisableGestureBack(const std::string &bundleName)
    {
    };

    /**
     * Set Title and Dock Hover Hide
     * @param bundleName The bundleName of the current game
     */
    virtual void SetTitleAndDockHoverHidden(const std::string &bundleName)
    {
    };

    /**
     * Set the visibility of the mouse
     * @param bundleName The bundleName of the current game
     * @param visible True means visible
     */
    virtual void SetPointerVisible(const std::string &bundleName, bool visible)
    {
    };
};

class SaEventCallback {
public:
    /**
     * Process the mapping switch for the current game
     * @param bundleName The bundleName of the current game
     * @param isEnabled True means enabled
     */
    virtual void HandleKeyMappingEnableChangeEvent(const std::string &bundleName, bool isEnabled)
    {
    };
};
}
}

#endif //GAME_CONTROLLER_PLUGIN_EVENT_CALLBACK_H
