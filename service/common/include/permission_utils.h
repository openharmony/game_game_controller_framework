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

#ifndef GAME_CONTROLLER_PERMISSION_UTILS_H
#define GAME_CONTROLLER_PERMISSION_UTILS_H

#include <singleton.h>

namespace OHOS {
namespace GameController {
class PermissionUtils : public std::enable_shared_from_this<PermissionUtils> {
DECLARE_DELAYED_SINGLETON(PermissionUtils)

public:
    /**
     * Check whether bundleName is the same as bundleName of the current invoker.
     * @param bundleName bundleName
     * @return true: Yes
     */
    virtual bool VerifyBundleNameIsValid(const std::string &bundleName, const int32_t uid);

    /**
     * Indicates whether the request is from the system SA.
     * @return true: Yes
     */
    virtual bool IsSACall() const;

    /**
     * Indicates whether the request is from the system app.
     * @return true: Yes
     */
    virtual bool IsSystemAppCall() const;
};
}
}

#endif //GAME_CONTROLLER_PERMISSION_UTILS_H
