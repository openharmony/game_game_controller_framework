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

#include "bundle_manager.h"
#include "system_ability_definition.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t BUNDLE_MANAGER_SA_ID = 401;
}

BundleManager::BundleManager()
{
}

BundleManager::~BundleManager()
{
}

BundleBasicInfo BundleManager::GetSelfBundleName()
{
    sptr<AppExecFwk::IBundleMgr> bundleMgr = GetBundleMgr();
    BundleBasicInfo bundle;
    if (bundleMgr == nullptr) {
        return bundle;
    }
    AppExecFwk::BundleInfo bundleInfo;
    int32_t flags = static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION);
    bundleMgr_->GetBundleInfoForSelf(flags, bundleInfo);
    AppExecFwk::ApplicationInfo appInfo = bundleInfo.applicationInfo;
    bundle.Convert(bundleInfo);
    HILOGI("The current bundleName [%{public}s]. version [%{public}s]", bundle.bundleName.c_str(),
           bundle.version.c_str());
    return bundle;
}

sptr<AppExecFwk::IBundleMgr> BundleManager::GetBundleMgr()
{
    if (bundleMgr_ != nullptr) {
        return bundleMgr_;
    }
    std::lock_guard<std::mutex> lock(createBundleMgrMute_);
    if (bundleMgr_ == nullptr) {
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityManager == nullptr) {
            HILOGE("get BundleMgrSystemAbilityManager failed");
            return bundleMgr_;
        }

        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MANAGER_SA_ID);
        if (remoteObject == nullptr) {
            HILOGE("get Bundle Manager failed");
            return bundleMgr_;
        }

        bundleMgr_ = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    }
    return bundleMgr_;
}
}
}