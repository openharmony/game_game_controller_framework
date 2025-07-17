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

#include <gtest/hwext/gtest-ext.h>
#include <gtest/hwext/gtest-tag.h>
#include <gmock/gmock-actions.h>
#include <gmock/gmock-spec-builders.h>
#include "gamecontroller_utils.h"
#include <gtest/gtest.h>
#include "refbase.h"

using ::testing::Return;
using namespace testing::ext;
namespace OHOS {
namespace GameController {
class GameControllerUtilsTest : public testing::Test {
public:
    void SetUp() override;

    void TearDown() override;
};

void GameControllerUtilsTest::SetUp()
{
}

void GameControllerUtilsTest::TearDown()
{
}

/**
* @tc.name: StringUtils_AnonymizationUniq_001
* @tc.desc: StringUtils_AnonymizationUniq_001
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(GameControllerUtilsTest, StringUtils_AnonymizationUniq_001, TestSize.Level0)
{
    ASSERT_EQ("**", StringUtils::AnonymizationUniq("11:22"));
    ASSERT_EQ("11:22:**", StringUtils::AnonymizationUniq("11:22:33"));
    ASSERT_EQ("11:22:**:**", StringUtils::AnonymizationUniq("11:22:33:44"));
    ASSERT_EQ("11:22:**:**:55", StringUtils::AnonymizationUniq("11:22:33:44:55"));
}
}
}