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

#include "window_info_manager.h"
#include "gamecontroller_log.h"
#include "key_to_touch_manager.h"
#include "key_mapping_service.h"

namespace OHOS {
namespace GameController {
using namespace OHOS::Rosen;

void WindowChangeListener::OnSizeChange(Rect rect, WindowSizeChangeReason reason,
                                        const std::shared_ptr<RSTransaction> &rsTransaction)
{
    HILOGI("OnSizeChange rect:%{public}s", rect.ToString().c_str());
    DelayedSingleton<WindowInfoManager>::GetInstance()->UpdateWindowInfo(rect);
}

void WindowChangeListener::OnModeChange(WindowMode mode, bool hasDeco)
{
    HILOGI("OnModeChange mode:%{public}d", mode);
}

void WindowLifeCycle::AfterForeground()
{
    HILOGI("AfterForeground ");
    DelayedSingleton<WindowInfoManager>::GetInstance()->SetForeground(true);
    DelayedSingleton<KeyMappingService>::GetInstance()->ReloadKeyMappingSupportConfig();
}

void WindowLifeCycle::AfterBackground()
{
    HILOGI("AfterBackground ");
    DelayedSingleton<WindowInfoManager>::GetInstance()->SetForeground(false);
}

void WindowLifeCycle::AfterFocused()
{
    HILOGI("AfterFocused ");
    DelayedSingleton<WindowInfoManager>::GetInstance()->SetFocus(true);
}

void WindowLifeCycle::AfterUnfocused()
{
    HILOGI("AfterUnfocused ");
    DelayedSingleton<WindowInfoManager>::GetInstance()->SetFocus(false);
}

void WindowLifeCycle::ForegroundFailed(int32_t ret)
{
    HILOGI("ForegroundFailed ");
}

void WindowLifeCycle::BackgroundFailed(int32_t ret)
{
    HILOGI("BackgroundFailed ");
}

void WindowLifeCycle::AfterActive()
{
    HILOGI("AfterActive ");
}

void WindowLifeCycle::AfterInactive()
{
    HILOGI("AfterInactive ");
}

void WindowLifeCycle::AfterResumed()
{
    HILOGI("AfterResumed ");
}

void WindowLifeCycle::AfterPaused()
{
    HILOGI("AfterPaused ");
}

void WindowLifeCycle::AfterDestroyed()
{
    HILOGI("AfterDestroyed ");
}

void WindowLifeCycle::AfterDidForeground()
{
    HILOGI("AfterDidForeground ");
}

void WindowLifeCycle::AfterDidBackground()
{
    HILOGI("AfterDidBackground ");
}

WindowInfoManager::WindowInfoManager()
{
}

WindowInfoManager::~WindowInfoManager()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (mainWindow_ == nullptr) {
        return;
    }
    if (windowChangeListener_ != nullptr) {
        mainWindow_->UnregisterWindowChangeListener(windowChangeListener_);
    }
    if (windowLifeCycle_ != nullptr) {
        mainWindow_->UnregisterLifeCycleListener(windowLifeCycle_);
    }

    mainWindow_ = nullptr;
    windowChangeListener_ = nullptr;
    windowLifeCycle_ = nullptr;
}

bool WindowInfoManager::InitWindowInfo(const std::string &bundleName)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string windowName = bundleName + std::to_string(0);
    std::size_t pos = windowName.find_last_of('.');
    windowName = (pos == std::string::npos) ? windowName : windowName.substr(pos + 1);
    HILOGI("main window's name is [%{public}s]", windowName.c_str());
    mainWindow_ = Window::Find(windowName);
    if (mainWindow_ == nullptr) {
        HILOGE("cannot find main window [%{public}s]", windowName.c_str());
        return false;
    }
    int32_t windowId = static_cast<int32_t>(mainWindow_->GetWindowId());
    HILOGI("windowId is [%{public}d]", windowId);
    Rect rect = mainWindow_->GetRect();
    HILOGI("window rect is  [%{public}s]", rect.ToString().c_str());
    initWindowInfoEntity_.windowId = windowId;
    initWindowInfoEntity_.ParseRect(rect);
    WindowMode windowMode = mainWindow_->GetWindowMode();
    initWindowInfoEntity_.isFullScreen = windowMode == Rosen::WindowMode::WINDOW_MODE_FULLSCREEN;
    HILOGI("window isFullScreen [%{public}d]", initWindowInfoEntity_.isFullScreen ? 1 : 0);
    initWindowInfoEntity_.maxWidth = static_cast<int32_t>(rect.width_);
    initWindowInfoEntity_.maxHeight = static_cast<int32_t>(rect.height_);
    DelayedSingleton<KeyToTouchManager>::GetInstance()->UpdateWindowInfo(initWindowInfoEntity_);
    windowChangeListener_ = sptr<WindowChangeListener>::MakeSptr();
    mainWindow_->RegisterWindowChangeListener(windowChangeListener_);
    windowLifeCycle_ = sptr<WindowLifeCycle>::MakeSptr();
    mainWindow_->RegisterLifeCycleListener(windowLifeCycle_);
    return true;
}

void WindowInfoManager::UpdateWindowInfo(const Rect &rect)
{
    WindowInfoEntity windowInfoEntity;
    windowInfoEntity.windowId = initWindowInfoEntity_.windowId;
    windowInfoEntity.ParseRect(rect);
    WindowMode windowMode = mainWindow_->GetWindowMode();
    HILOGI("windowMode is [%{public}d]", windowMode);
    bool isFullScreen = false;
    if (windowMode == Rosen::WindowMode::WINDOW_MODE_FULLSCREEN) {
        initWindowInfoEntity_.maxWidth = windowInfoEntity.currentWidth;
        initWindowInfoEntity_.maxHeight = windowInfoEntity.currentHeight;
        isFullScreen = true;
    }
    windowInfoEntity.maxWidth = initWindowInfoEntity_.maxWidth;
    windowInfoEntity.maxHeight = initWindowInfoEntity_.maxHeight;
    windowInfoEntity.isFullScreen = isFullScreen;
    DelayedSingleton<KeyToTouchManager>::GetInstance()->UpdateWindowInfo(windowInfoEntity);
}

void WindowInfoManager::SetForeground(bool isForeground)
{
    std::lock_guard<std::mutex> lock(mutex_);
    isForeground_ = isForeground;
}

void WindowInfoManager::SetFocus(bool isFocus)
{
    std::lock_guard<std::mutex> lock(mutex_);
    isFocus_ = isFocus;
}

bool WindowInfoManager::IsForegroundAndFocus()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return isForeground_ && isFocus_;
}

void WindowInfoManager::DisableGestureBack()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (mainWindow_ == nullptr) {
        return;
    }
    bool currentIsEnable = false;
    mainWindow_->GetGestureBackEnabled(currentIsEnable);
    isEnableGestureBack_ = currentIsEnable;
    HILOGI("the oriGestureBackEnabled is [%{public}d]", isEnableGestureBack_ ? 1 : 0);
    mainWindow_->SetGestureBackEnabled(false);
}

void WindowInfoManager::EnableGestureBackEnabled()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (mainWindow_ == nullptr) {
        return;
    }
    if (isEnableGestureBack_) {
        HILOGI("EnableGestureBack");
        mainWindow_->SetGestureBackEnabled(true);
    }
}

void WindowInfoManager::SetTitleAndDockHoverShown()
{
    std::lock_guard<std::mutex> lock(mutex_);
    HILOGI("EnableGestureBack");
    if (mainWindow_ == nullptr) {
        return;
    }
    mainWindow_->SetTitleAndDockHoverShown(false, false);
}
}
}