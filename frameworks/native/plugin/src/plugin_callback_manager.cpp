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
#include <window_input_intercept_client.h>
#include "plugin_callback_manager.h"
#include "window_opr_handle.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {

PluginCallbackManager::~PluginCallbackManager()
{
}

PluginCallbackManager::PluginCallbackManager()
{
    callbackQueue_ = std::make_unique<ffrt::queue>("plugin-callback-thread",
                                                   ffrt::queue_attr().qos(ffrt::qos_background));
    windowOprCallbackPtr_ = std::make_shared<WindowOprHandle>();
}

void PluginCallbackManager::RegisterDeviceCallBack(const std::shared_ptr<DeviceCallback> &deviceCallback)
{
    std::lock_guard<ffrt::mutex> lock(locker_);
    if (deviceCallbackPtr_ == nullptr) {
        deviceCallbackPtr_ = deviceCallback;
    }
}

void PluginCallbackManager::RegisterSaEventCallback(const std::shared_ptr<SaEventCallback> &saEventCallback)
{
    std::lock_guard<ffrt::mutex> lock(locker_);
    if (saEventCallbackPtr_ == nullptr) {
        saEventCallbackPtr_ = saEventCallback;
    }
}

void PluginCallbackManager::OnDeviceStatusChanged(const std::string &bundleName, const DeviceInfo &deviceInfo)
{
    if (!isPluginMode_) {
        return;
    }
    callbackQueue_->submit([bundleName, deviceInfo, this] {
        std::lock_guard<ffrt::mutex> lock(locker_);
        if (deviceCallbackPtr_ == nullptr) {
            return;
        }
        deviceCallbackPtr_->OnDeviceStatusChanged(bundleName, deviceInfo);
    });
}

void PluginCallbackManager::HandleKeyMappingEnableChangeEvent(const std::string &bundleName, bool isEnabled)
{
    if (!isPluginMode_) {
        return;
    }
    callbackQueue_->submit([bundleName, isEnabled, this] {
        std::lock_guard<ffrt::mutex> lock(locker_);
        if (saEventCallbackPtr_ == nullptr) {
            return;
        }
        saEventCallbackPtr_->HandleKeyMappingEnableChangeEvent(bundleName, isEnabled);
    });
}

void PluginCallbackManager::SetPluginMode(bool isPluginMode)
{
    isPluginMode_ = isPluginMode;
}

void PluginCallbackManager::RegisterWindowOprCallback(const std::shared_ptr<WindowOprCallback> &windowOprCallback)
{
    std::lock_guard<ffrt::mutex> lock(locker_);
    windowOprCallbackPtr_ = windowOprCallback;
    HILOGI("RegisterWindowOprCallback success");
}

void PluginCallbackManager::SendInputEvent(const std::string &bundleName,
                                           const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                           bool isSimulate)
{
    if (isPluginMode_) {
        callbackQueue_->submit([bundleName, keyEvent, isSimulate, this] {
            std::lock_guard<ffrt::mutex> lock(locker_);
            if (windowOprCallbackPtr_ == nullptr) {
                return;
            }
            if (!windowOprCallbackPtr_->SendInputEvent(bundleName, keyEvent, isSimulate)) {
                Rosen::WindowInputInterceptClient::SendInputEvent(keyEvent);
            }
        });
    } else {
        std::lock_guard<ffrt::mutex> lock(locker_);
        windowOprCallbackPtr_->SendInputEvent(bundleName, keyEvent, isSimulate);
    }
}

void PluginCallbackManager::SendInputEvent(const std::string &bundleName,
                                           const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                           bool isSimulate)
{
    if (isPluginMode_) {
        callbackQueue_->submit([bundleName, pointerEvent, isSimulate, this] {
            std::lock_guard<ffrt::mutex> lock(locker_);
            if (windowOprCallbackPtr_ == nullptr) {
                return;
            }
            if (!windowOprCallbackPtr_->SendInputEvent(bundleName, pointerEvent, isSimulate)) {
                Rosen::WindowInputInterceptClient::SendInputEvent(pointerEvent);
            }
        });
    } else {
        std::lock_guard<ffrt::mutex> lock(locker_);
        windowOprCallbackPtr_->SendInputEvent(bundleName, pointerEvent, isSimulate);
    }
}

void PluginCallbackManager::RecoverGestureBackStatus(const std::string &bundleName)
{
    if (isPluginMode_) {
        callbackQueue_->submit([bundleName, this] {
            std::lock_guard<ffrt::mutex> lock(locker_);
            if (windowOprCallbackPtr_ == nullptr) {
                return;
            }
            windowOprCallbackPtr_->RecoverGestureBackStatus(bundleName);
        });
    } else {
        std::lock_guard<ffrt::mutex> lock(locker_);
        windowOprCallbackPtr_->RecoverGestureBackStatus(bundleName);
    }
}

void PluginCallbackManager::DisableGestureBack(const std::string &bundleName)
{
    if (isPluginMode_) {
        callbackQueue_->submit([bundleName, this] {
            std::lock_guard<ffrt::mutex> lock(locker_);
            if (windowOprCallbackPtr_ == nullptr) {
                return;
            }
            windowOprCallbackPtr_->DisableGestureBack(bundleName);
        });
    } else {
        std::lock_guard<ffrt::mutex> lock(locker_);
        windowOprCallbackPtr_->DisableGestureBack(bundleName);
    }
}

void PluginCallbackManager::SetTitleAndDockHoverHidden(const std::string &bundleName)
{
    if (isPluginMode_) {
        callbackQueue_->submit([bundleName, this] {
            std::lock_guard<ffrt::mutex> lock(locker_);
            if (windowOprCallbackPtr_ == nullptr) {
                return;
            }
            windowOprCallbackPtr_->SetTitleAndDockHoverHidden(bundleName);
        });
    } else {
        std::lock_guard<ffrt::mutex> lock(locker_);
        windowOprCallbackPtr_->SetTitleAndDockHoverHidden(bundleName);
    }
}

void PluginCallbackManager::SetPointerVisible(const std::string &bundleName, bool visible)
{
    if (isPluginMode_) {
        callbackQueue_->submit([bundleName, visible, this] {
            std::lock_guard<ffrt::mutex> lock(locker_);
            if (windowOprCallbackPtr_ == nullptr) {
                return;
            }
            windowOprCallbackPtr_->SetPointerVisible(bundleName, visible);
        });
    } else {
        std::lock_guard<ffrt::mutex> lock(locker_);
        windowOprCallbackPtr_->SetPointerVisible(bundleName, visible);
    }
}

}
}