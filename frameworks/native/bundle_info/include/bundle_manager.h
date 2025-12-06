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
#ifndef GAME_CONTROLLER_FRAMEWORK_BUNDLE_MANAGER_H
#define GAME_CONTROLLER_FRAMEWORK_BUNDLE_MANAGER_H

#include <singleton.h>
#include <bundle_mgr_interface.h>

namespace OHOS {
namespace GameController {
struct BundleBasicInfo {
    std::string bundleName;
    std::string version;
    bool isCommonApp;

    void Convert(const AppExecFwk::BundleInfo &bundleInfo)
    {
        bundleName = bundleInfo.name;
        AppExecFwk::ApplicationInfo appInfo = bundleInfo.applicationInfo;
        version = bundleInfo.versionName;
        isCommonApp = (appInfo.bundleType == AppExecFwk::BundleType::APP && !appInfo.isLauncherApp
            && !appInfo.isSystemApp);
    }

    /**
     * 应用为非系统应用并且类型为APP类型时，需要启动事件订阅
     * @return true表示启动事件订阅
     */
    bool IsCommonApp()
    {
        return isCommonApp;
    }
};

class BundleManager : public DelayedSingleton<BundleManager> {
DECLARE_DELAYED_SINGLETON(BundleManager)

public:
    /**
     * Obtains the self bundle name
     * @return BundleInfo
     */
    BundleBasicInfo GetSelfBundleName();

private:
    sptr <AppExecFwk::IBundleMgr> GetBundleMgr();

private:
    std::mutex createBundleMgrMute_;

    sptr <AppExecFwk::IBundleMgr> bundleMgr_ = nullptr;
};
}
}
#endif //GAME_CONTROLLER_FRAMEWORK_BUNDLE_MANAGER_H
