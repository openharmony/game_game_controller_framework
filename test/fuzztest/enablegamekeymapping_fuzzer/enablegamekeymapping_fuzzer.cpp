/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cstdint>
#include "enablegamekeymapping_fuzzer.h"
#include "gamecontroller_constants.h"
#include "gamecontroller_log.h"
#include "event_publisher.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t MAX_CONFIG_NUM = 200;
}

void EnableGameKeyMapping(const uint8_t* rawData, size_t size)
{
    GameInfo gameInfo;
    gameInfo.bundleName = std::string(reinterpret_cast<const char*>(rawData), size)
        .substr(0, MAX_BUNDLE_NAME_LENGTH);
    gameInfo.version = std::string(reinterpret_cast<const char*>(rawData), size)
        .substr(0, MAX_VERSION_LENGTH);
    bool isEnable = static_cast<bool>(*rawData);
    DelayedSingleton<EventPublisher>::GetInstance()->SendEnableGameKeyMappingNotify(gameInfo, isEnable);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GameController::EnableGameKeyMapping(data, size);
    return 0;
}