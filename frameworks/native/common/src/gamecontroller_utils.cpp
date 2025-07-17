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

#include <cstring>
#include <cstdint>
#include <vector>
#include <securec.h>
#include <sstream>
#include "gamecontroller_utils.h"
#include "gamecontroller_errors.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
namespace {
const char DEVICE_DELIMITER = ':';
const int32_t START_IDX = 2;
const int32_t END_IDX = 3;
}

int32_t StringUtils::ConvertToCharPtrArray(const std::string &string, char** result)
{
    int32_t count = static_cast<int32_t>(string.size());
    *result = static_cast<char*>(malloc(sizeof(char) * (count + 1)));
    if (*result == nullptr) {
        HILOGI("malloc is feiled!");
        return GAME_ERR_ENOMEM;
    }
    int ret = strncpy_s(*result, count + 1, string.c_str(), count);
    if (ret != 0) {
        HILOGI("strncpy_s is failed!");
        free(*result);
        *result = nullptr;
        return GAME_ERR_ENOMEM;
    }
    return GAME_CONTROLLER_SUCCESS;
}

std::string StringUtils::AnonymizationUniq(const std::string &uniq)
{
    std::vector<std::string> fields;
    std::stringstream ss(uniq);
    std::string field;
    while (std::getline(ss, field, DEVICE_DELIMITER)) {
        fields.push_back(field);
    }

    int32_t size = static_cast<int32_t>(fields.size());
    if (size <= START_IDX) {
        return "**";
    }
    std::string temp;
    for (int32_t idx = 0; idx < size; idx++) {
        if (idx != 0) {
            temp.push_back(DEVICE_DELIMITER);
        }
        if (idx >= START_IDX && idx <= END_IDX) {
            temp.append("**");
        } else {
            temp.append(fields[idx]);
        }
    }
    return temp;
}
}
}