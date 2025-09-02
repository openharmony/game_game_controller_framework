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

#ifndef GAME_CONTROLLER_PERMISSION_UTILS_MOCK_H
#define GAME_CONTROLLER_PERMISSION_UTILS_MOCK_H
namespace OHOS {
namespace GameController {
class GameControllerServerAbilityEx : public GameControllerServerAbility {
public:
    explicit GameControllerServerAbilityEx(int32_t systemAbilityId, bool runOnCreate = false)
        : GameControllerServerAbility(systemAbilityId, runOnCreate)
    {
    }

    bool IsSystemServiceCall() override
    {
        return true;
    }

    bool VerifyBundleNameIsValid(const std::string &bundleName) override
    {
        return true;
    }

    bool IsSupportGameKeyMapping(const GameInfo &gameInfo) override
    {
        return true;
    }
};

}
}

#endif //GAME_CONTROLLER_PERMISSION_UTILS_MOCK_H
