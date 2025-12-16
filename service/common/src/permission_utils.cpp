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

#include <ipc_skeleton.h>
#include <accesstoken_kit.h>
#include <tokenid_kit.h>
#include <bundle_mgr_client.h>
#include <errors.h>
#include "permission_utils.h"
#include "gamecontroller_log.h"

using namespace OHOS::AppExecFwk;
using namespace OHOS::Security::AccessToken;

namespace OHOS {
namespace GameController {
PermissionUtils::~PermissionUtils()
{
}

PermissionUtils::PermissionUtils()
{
}

bool PermissionUtils::VerifyBundleNameIsValid(const std::string &bundleName, const int32_t uid)
{
    std::string tempBundleName = "";
    BundleMgrClient bundleObj;
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    ErrCode res = bundleObj.GetNameForUid(uid, tempBundleName);
    IPCSkeleton::SetCallingIdentity(identity);
    if (res != ERR_OK) {
        HILOGE("Failed to get bundle name, ErrCode=%{public}d", static_cast<int32_t>(res));
        return false;
    }
    return tempBundleName == bundleName;
}

bool PermissionUtils::IsSACall() const
{
    auto callerToken = IPCSkeleton::GetCallingTokenID();
    auto tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    return tokenType == ATokenTypeEnum::TOKEN_NATIVE;
}

bool PermissionUtils::IsSystemAppCall() const
{
    auto callerToken = IPCSkeleton::GetCallingFullTokenID();
    return TokenIdKit::IsSystemAppByFullTokenID(callerToken);
}
}
}