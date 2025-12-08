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

#ifndef GAME_CONTROLLER_FRAMEWORK_WINDOW_OPR_HANDLE_H
#define GAME_CONTROLLER_FRAMEWORK_WINDOW_OPR_HANDLE_H

#include <singleton.h>
#include "plugin_event_callback.h"

namespace OHOS {
namespace GameController {
class WindowOprHandle : public WindowOprCallback {
public:
    WindowOprHandle() = default;

    virtual ~WindowOprHandle()
    {
    }

    bool SendInputEvent(const std::string &bundleName, const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                        bool isSimulate) override;

    bool SendInputEvent(const std::string &bundleName, const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                        const bool isSimulate) override;

    void RecoverGestureBackStatus(const std::string &bundleName) override;

    void DisableGestureBack(const std::string &bundleName) override;

    void SetTitleAndDockHoverHidden(const std::string &bundleName) override;

    void SetPointerVisible(const std::string &bundleName, const bool visible) override;
};
}
}
#endif //GAME_CONTROLLER_FRAMEWORK_WINDOW_OPR_HANDLE_H
