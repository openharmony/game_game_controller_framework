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

#ifndef GAME_CONTROLLER_FRAMEWORK_WINDOW_INFO_MANGER_H
#define GAME_CONTROLLER_FRAMEWORK_WINDOW_INFO_MANGER_H

#include <singleton.h>
#include <window.h>
#include "key_to_touch_manager.h"

namespace OHOS {
namespace GameController {
using namespace OHOS::Rosen;

class WindowChangeListener : public IWindowChangeListener {
public:
    /**
     * @brief Notify caller when window size changed.
     *
     * @param Rect Rect of the current window.
     * @param reason Reason for window change.
     * @param rsTransaction Synchronization transaction for animation
     */
    virtual void OnSizeChange(OHOS::Rosen::Rect rect, WindowSizeChangeReason reason,
                              const std::shared_ptr<RSTransaction> &rsTransaction = nullptr) override;

    /**
     * @brief Notify caller when window mode changed.
     *
     * @param mode Mode of the current window.
     * @param hasDeco Window has decoration or not.
     */
    virtual void OnModeChange(WindowMode mode, bool hasDeco = true) override;
};

class WindowLifeCycle : public IWindowLifeCycle {
public:
    /**
     * @brief Notify caller that window is on the forground.
     */
    virtual void AfterForeground() override;

    /**
     * @brief Notify caller that window is on the background.
     */
    virtual void AfterBackground() override;

    /**
     * @brief Notify caller that window is focused.
     */
    virtual void AfterFocused() override;

    /**
     * @brief Notify caller that window is unfocused.
     */
    virtual void AfterUnfocused() override;

    /**
     * @brief Notify caller the error code when window go forground failed.
     *
     * @param ret Error code when window go forground failed.
     */
    virtual void ForegroundFailed(int32_t ret) override;

    /**
     * @brief Notify caller the error code when window go background failed.
     *
     * @param ret Error code when window go background failed.
     */
    virtual void BackgroundFailed(int32_t ret) override;

    /**
     * @brief Notify caller that window is active.
     */
    virtual void AfterActive() override;

    /**
     * @brief Notify caller that window is inactive.
     */
    virtual void AfterInactive() override;

    /**
     * @brief Notify caller that window is resumed.
     */
    virtual void AfterResumed() override;

    /**
     * @brief Notify caller that window is paused.
     */
    virtual void AfterPaused() override;

    /**
     * @brief Notify caller that window is destroyed.
     */
    virtual void AfterDestroyed() override;

    /**
     * @brief Notify caller that window is already foreground.
     */
    virtual void AfterDidForeground() override;

    /**
     * @brief Notify caller that window is already background.
     */
    virtual void AfterDidBackground() override;
};

class WindowInfoManager : public DelayedSingleton<WindowInfoManager> {
DECLARE_DELAYED_SINGLETON(WindowInfoManager);
public:
    /**
     * Init window information
     * @param bundleName bundle name
     * @return true means init success
     */
    bool InitWindowInfo(const std::string &bundleName);

    /**
     * Update window information OnSizeChange
     * @param rect Rect
     */
    void UpdateWindowInfo(const OHOS::Rosen::Rect &rect);

    void SetForeground(bool isForeground);

    void SetFocus(bool isFocus);

    /**
     * Is it on the front and in focus
     * @return true means yes.
     */
    bool IsForegroundAndFocus();

    void DisableGestureBack();

    void EnableGestureBack();

    void SetTitleAndDockHoverShown();

    int32_t GetWindowId();

private:
    std::mutex mutex_;
    std::string bundleName_;
    sptr<Window> mainWindow_ = nullptr;
    sptr<IWindowChangeListener> windowChangeListener_ = nullptr;
    sptr<IWindowLifeCycle> windowLifeCycle_ = nullptr;
    WindowInfoEntity initWindowInfoEntity_;
    bool isForeground_{true};
    bool isFocus_{true};
    bool isEnableGestureBack_{true};
};
}
}
#endif //GAME_CONTROLLER_FRAMEWORK_WINDOW_INFO_MANGER_H
