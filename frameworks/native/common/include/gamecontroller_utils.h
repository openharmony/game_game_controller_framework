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

#ifndef GAME_CONTROLLER_FRAMEWORK_GAME_CONTROLLER_UTILS_H
#define GAME_CONTROLLER_FRAMEWORK_GAME_CONTROLLER_UTILS_H

#include <string>

namespace OHOS {
namespace GameController {
class StringUtils {
public:
    /**
     * Convert a character string to char**
     * @param string String
     * @param result char**
     * @return The value 0 indicates that the conversion is successful.
     */
    static int32_t ConvertToCharPtrArray(const std::string &string, char** result);

    /**
     * Anonymize uniq.
     * @param uniq uniq
     * @return **.**.00.00
     */
    static std::string AnonymizationUniq(const std::string &uniq);
};
}
}

#endif //GAME_CONTROLLER_FRAMEWORK_GAME_CONTROLLER_UTILS_H
