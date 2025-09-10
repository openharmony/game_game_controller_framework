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
#include <fstream>
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <filesystem>
#include <dirent.h>
#include <regex>
#include "json_utils.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t ONE_BYTE_CHAR = 1;
const int32_t TOW_BYTE_CHAR = 2;
const int32_t THREE_BYTE_CHAR = 3;
const int32_t FOUR_BYTE_CHAR = 4;
const char UTF_ONE_BYTE_END = 0x7F;
const char UTF_TWO_BYTE_START = 0xC2;
const char UTF_TWO_BYTE_END = 0xDF;
const char UTF_THREE_BYTE_START = 0xE0;
const char UTF_THREE_BYTE_END = 0xEF;
const char UTF_THREE_SECOND_BYTE_LOW_BOUND = 0xA0;
const char UTF_THREE_ED_BYTE_START = 0xED;
const char UTF_THREE_ED_SECOND_BYTE_UP_BOUND = 0x9F;
const char UTF_FOUR_BYTE_START = 0xF0;
const char UTF_FOUR_BYTE_END = 0xF4;
const char UTF_FOUR_SECOND_BYTE_LOW_BOUND = 0x90;
const char UTF_FOUR_F4_SECOND_BYTE_UP_BOUND = 0x8F;
const char NON_ASCII_START = 0xC0;
const char MULTI_BYTE_START = 0x80;

bool TwoBytes(unsigned char c, std::string::const_iterator &it, std::string::const_iterator end)
{
    if (std::distance(it, end) < TOW_BYTE_CHAR) {
        return false;
    }
    if ((*(it + ONE_BYTE_CHAR) & NON_ASCII_START) != MULTI_BYTE_START) {
        return false;
    }
    it += TOW_BYTE_CHAR;
    return true;
}

bool ThreeBytes(unsigned char c, std::string::const_iterator &it, std::string::const_iterator end)
{
    if (std::distance(it, end) < THREE_BYTE_CHAR) {
        return false;
    }
    if ((*(it + ONE_BYTE_CHAR) & NON_ASCII_START) != MULTI_BYTE_START ||
        (*(it + TOW_BYTE_CHAR) & NON_ASCII_START) != MULTI_BYTE_START) {
        return false;
    }
    if (c == UTF_THREE_BYTE_START && *(it + ONE_BYTE_CHAR) < UTF_THREE_SECOND_BYTE_LOW_BOUND) {
        return false;
    }
    if (c == UTF_THREE_ED_BYTE_START && *(it + ONE_BYTE_CHAR) > UTF_THREE_ED_SECOND_BYTE_UP_BOUND) {
        return false;
    }
    it += THREE_BYTE_CHAR;
    return true;
}

bool FourBytes(unsigned char c, std::string::const_iterator &it, std::string::const_iterator end)
{
    if (std::distance(it, end) < FOUR_BYTE_CHAR) {
        return false;
    }
    if ((*(it + ONE_BYTE_CHAR) & NON_ASCII_START) != MULTI_BYTE_START ||
        (*(it + TOW_BYTE_CHAR) & NON_ASCII_START) != MULTI_BYTE_START ||
        (*(it + THREE_BYTE_CHAR) & NON_ASCII_START) != MULTI_BYTE_START) {
        return false;
    }
    if (c == UTF_FOUR_BYTE_START && *(it + ONE_BYTE_CHAR) < UTF_FOUR_SECOND_BYTE_LOW_BOUND) {
        return false;
    }
    if (c == UTF_FOUR_BYTE_END && *(it + ONE_BYTE_CHAR) > UTF_FOUR_F4_SECOND_BYTE_UP_BOUND) {
        return false;
    }
    it += FOUR_BYTE_CHAR;
    return true;
}
}

bool JsonUtils::IsUtf8(const std::string &str)
{
    auto it = str.begin();
    auto end = str.end();
    while (it != end) {
        unsigned char c = *it;
        if (c <= UTF_ONE_BYTE_END) {
            // Single-byte character, proceed to the next byte.
            ++it;
        } else if (c >= UTF_TWO_BYTE_START && c <= UTF_TWO_BYTE_END) {
            // Two-byte character, check the next byte
            if (!TwoBytes(c, it, end)) {
                return false;
            }
        } else if (c >= UTF_THREE_BYTE_START && c <= UTF_THREE_BYTE_END) {
            // Three-byte character, check the next two bytes.
            if (!ThreeBytes(c, it, end)) {
                return false;
            }
        } else if (c >= UTF_FOUR_BYTE_START && c <= UTF_FOUR_BYTE_END) {
            // Four-byte character, check the next three bytes.
            if (!FourBytes(c, it, end)) {
                return false;
            }
        } else {
            // Invalid UTF-8 bytes.
            return false;
        }
    }
    return true;
}

bool JsonUtils::WriteFileFromJson(const std::string &path, const nlohmann::json &jsonContent)
{
    char canonicalPath[PATH_MAX] = {};
    if (realpath(path.c_str(), canonicalPath) == nullptr) {
        HILOGE("Invalid path [%{public}s].", path.c_str());
        return false;
    }

    std::ofstream outfile(canonicalPath);
    if (outfile.is_open()) {
        HILOGI("open [%{public}s] file success.", canonicalPath);
        outfile << jsonContent;
        outfile.close();
        return true;
    } else {
        HILOGE("open [%{public}s] file failed.", canonicalPath);
        return false;
    }
}

std::pair<bool, nlohmann::json> JsonUtils::ReadJsonFromFile(const std::string &path)
{
    nlohmann::json content;
    char canonicalPath[PATH_MAX] = {};
    if (realpath(path.c_str(), canonicalPath) == nullptr) {
        HILOGE("realpath failed for [%{public}s]", path.c_str());
        return std::make_pair(false, content);
    }

    std::ifstream ifs(canonicalPath, std::ios::binary);
    if (!ifs.is_open()) {
        HILOGE("open [%{public}s] file failed.", canonicalPath);
        return std::make_pair(false, content);
    }

    content = nlohmann::json::parse(ifs, 0, false);
    if (content.is_discarded()) {
        HILOGE("parse [%{public}s] file failed.", canonicalPath);
        ifs.close();
        return std::make_pair(false, content);
    }

    if (ifs.fail() || ifs.bad()) {
        HILOGE("load [%{public}s] file failed.", canonicalPath);
        ifs.close();
        return std::make_pair(false, content);
    }

    HILOGI("load [%{public}s] file success.", canonicalPath);
    ifs.close();
    return std::make_pair(true, content);
}

bool JsonUtils::IsFileExist(const std::string &filePath)
{
    return access(filePath.c_str(), F_OK) == 0;
}

bool JsonUtils::CopyFile(const std::string &src, const std::string &dest, const bool isAllUserRead)
{
    char canonicalPath[PATH_MAX] = {};
    auto srcRealPath = realpath(src.c_str(), canonicalPath);
    if (srcRealPath == nullptr) {
        HILOGE("get src realpath faild");
        return false;
    }
    std::ifstream srcFile(srcRealPath);
    if (!srcFile.is_open()) {
        HILOGE("open srcFile file failed");
        free(srcRealPath);
        return false;
    }

    std::ofstream destFile(dest, std::ios_base::out);
    if (!destFile.is_open()) {
        HILOGE("open destFile file failed");
        srcFile.close();
        free(srcRealPath);
        return false;
    }
    destFile << srcFile.rdbuf();

    srcFile.close();
    if (destFile.fail()) {
        destFile.clear();
        destFile.close();
        free(srcRealPath);
        HILOGE("write destFile failed");
        return false;
    }
    destFile.flush();
    destFile.close();
    free(srcRealPath);
    int res;
    if (isAllUserRead) {
        res = chmod(dest.c_str(), S_IREAD | S_IWRITE | S_IRGRP | S_IROTH);
    } else {
        res = chmod(dest.c_str(), S_IREAD | S_IWRITE);
    }
    if (res != 0) {
        HILOGE("chmod config file failed: %{public}d", res);
    }
    return true;
}

int32_t JsonUtils::GetJsonInt32Value(const nlohmann::json &jsonObj, const std::string &fieldName, int32_t defaultVal)
{
    if (jsonObj.contains(fieldName) && jsonObj.at(fieldName).is_number_integer()) {
        return jsonObj.at(fieldName).get<int32_t>();
    }
    return defaultVal;
}
}
}