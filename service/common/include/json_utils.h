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

#ifndef GAME_CONTROLLER_JSON_UTILS_H
#define GAME_CONTROLLER_JSON_UTILS_H

#include <string>
#include "nlohmann/json.hpp"

namespace OHOS {
namespace GameController {

const std::string GAME_CONTROLLER_ETC_ROOT = "/system/etc/game_controller/game_controller_service/";
const std::string GAME_CONTROLLER_SERVICE_ROOT = "/data/service/el1/public/gamecontroller_server/";

class JsonUtils {
public:
    /**
     * JSON only supports UTF-8.
     * @param str string
     * @return true means it's utf-8
     */
    static bool IsUtf8(const std::string &str);

    /**
     * Write the JSON object to the file.
     * @param path File Path
     * @param jsonContent JSON content
     * @return The value true indicates that the data is successfully written.
     */
    static bool WriteFileFromJson(const std::string &path, const nlohmann::json &jsonContent);

    /**
     * Read the JSON object from the file.
     * @param path File Path
     * @return pair.first indicates whether the operation is successful,
     * and pair.second indicates the JSON content to be read.
     */
    static std::pair<bool, nlohmann::json> ReadJsonFromFile(const std::string &path);

    /**
     * Check whether the file exists.
     * @param filePath file path
     * @return true the file exists.
     */
    static bool IsFileExist(const std::string &filePath);

    /**
     * Copying Files
     * @param src Source Path
     * @param dest Target Path
     * @param isAllUserRead whether it's necessary for all users to have read access
     * @return true: The copy is successful.
     */
    static bool CopyFile(const std::string &src, const std::string &dest, const bool isAllUserRead);

    static int32_t GetJsonInt32Value(const nlohmann::json &jsonObj, const std::string &fieldName, int32_t defaultVal);
};
}
}
#endif //GAME_CONTROLLER_JSON_UTILS_H
