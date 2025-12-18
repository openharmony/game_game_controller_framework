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
#include <input_manager.h>
#include "window_opr_handle.h"
#include "window_info_manager.h"

namespace OHOS {
namespace GameController {

bool WindowOprHandle::SendInputEvent(const std::string &bundleName, const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                                     const bool isSimulate)
{
    Rosen::WindowInputInterceptClient::SendInputEvent(keyEvent);
    return true;
}

bool WindowOprHandle::SendInputEvent(const std::string &bundleName,
                                     const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                     const bool isSimulate)
{
    Rosen::WindowInputInterceptClient::SendInputEvent(pointerEvent);
    return true;
}

void WindowOprHandle::RecoverGestureBackStatus(const std::string &bundleName)
{
    DelayedSingleton<WindowInfoManager>::GetInstance()->EnableGestureBack();
}

void WindowOprHandle::DisableGestureBack(const std::string &bundleName)
{
    DelayedSingleton<WindowInfoManager>::GetInstance()->DisableGestureBack();
}

void WindowOprHandle::SetTitleAndDockHoverHidden(const std::string &bundleName)
{
    DelayedSingleton<WindowInfoManager>::GetInstance()->SetTitleAndDockHoverShown();
}

void WindowOprHandle::SetPointerVisible(const std::string &bundleName, const bool visible)
{
    InputManager::GetInstance()->SetPointerVisible(visible, 0);
}
}
}