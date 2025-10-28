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

#include <fstream>
#include <sys/stat.h>
#include "gtest/hwext/gtest-ext.h"
#include "gtest/hwext/gtest-tag.h"

#define private public

#include "json_utils.h"

#undef private

#include <gtest/gtest.h>
#include <string>
#include "refbase.h"

using namespace testing::ext;
using namespace std;
using json = nlohmann::json;

namespace OHOS {
namespace GameController {
namespace {
const int32_t MAX_DEVICE_NUMBER = 10000;
const char UTF_ONE_BYTE_END = 0x7F;
const char UTF_TWO_BYTE_START_WRONG = 0xC1;
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
const char MULTI_BYTE_END = 0xBF;
const std::string TEST_FILE_NAME = "_test_.txt";
const std::string COPY_FILE_NAME = "_copy_.txt";
const std::string ALL_USER_READ_MODE = "rw-r--r--";
const std::string OWNER_ONLY_MODE = "rw-------";
}
class JsonUtilsTest : public testing::Test {
public:
    void SetUp();

    void TearDown();

    std::string TwoByte();

    std::string TwoByteWrong();

    std::string ThreeByte();

    std::string ThreeByteWrong();

    std::string ThreeByteWrong2();

    std::string FourByte();

    std::string FourByteWrong();

    std::string FourByteWrong2();

    void PreBuildTestFile(const string &filename);

    void RemoveTestFile(const string &filepath);

    std::string FileMode(struct stat fileStat);
};

void JsonUtilsTest::SetUp()
{
    PreBuildTestFile(TEST_FILE_NAME);
}

void JsonUtilsTest::TearDown()
{
    RemoveTestFile(TEST_FILE_NAME);
}

std::string JsonUtilsTest::TwoByte()
{
    std::string testStr;
    testStr.push_back(UTF_TWO_BYTE_START);
    testStr.push_back(MULTI_BYTE_END);
    return testStr;
}

std::string JsonUtilsTest::TwoByteWrong()
{
    std::string testStr;
    testStr.push_back(UTF_TWO_BYTE_START_WRONG);
    testStr.push_back(MULTI_BYTE_START);
    return testStr;
}

std::string JsonUtilsTest::ThreeByte()
{
    std::string testStr;
    testStr.push_back(UTF_THREE_BYTE_START);
    testStr.push_back(MULTI_BYTE_END);
    testStr.push_back(MULTI_BYTE_END);
    return testStr;
}

std::string JsonUtilsTest::ThreeByteWrong()
{
    std::string testStr;
    testStr.push_back(UTF_THREE_BYTE_START);
    testStr.push_back(UTF_THREE_ED_SECOND_BYTE_UP_BOUND);
    testStr.push_back(MULTI_BYTE_START);
    return testStr;
}

std::string JsonUtilsTest::ThreeByteWrong2()
{
    std::string testStr;
    testStr.push_back(UTF_THREE_ED_BYTE_START);
    testStr.push_back(UTF_THREE_SECOND_BYTE_LOW_BOUND);
    testStr.push_back(MULTI_BYTE_START);
    return testStr;
}

std::string JsonUtilsTest::FourByte()
{
    std::string testStr;
    testStr.push_back(UTF_FOUR_BYTE_START);
    testStr.push_back(MULTI_BYTE_END);
    testStr.push_back(MULTI_BYTE_END);
    testStr.push_back(MULTI_BYTE_END);
    return testStr;
}

std::string JsonUtilsTest::FourByteWrong()
{
    std::string testStr;
    testStr.push_back(UTF_FOUR_BYTE_START);
    testStr.push_back(UTF_FOUR_F4_SECOND_BYTE_UP_BOUND);
    testStr.push_back(MULTI_BYTE_START);
    testStr.push_back(MULTI_BYTE_START);
    return testStr;
}

std::string JsonUtilsTest::FourByteWrong2()
{
    std::string testStr;
    testStr.push_back(UTF_FOUR_BYTE_END);
    testStr.push_back(UTF_FOUR_SECOND_BYTE_LOW_BOUND);
    testStr.push_back(MULTI_BYTE_START);
    testStr.push_back(MULTI_BYTE_START);
    return testStr;
}

void JsonUtilsTest::PreBuildTestFile(const string &filename)
{
    std::ofstream file(filename);
    if (!file) {
        return;
    }
    file.close();
    return;
}

void JsonUtilsTest::RemoveTestFile(const string &filepath)
{
    remove(filepath.c_str());
}

std::string JsonUtilsTest::FileMode(struct stat fileStat)
{
    string fileMode;
    fileMode += (fileStat.st_mode & S_IRUSR) ? "r" : "-";
    fileMode += (fileStat.st_mode & S_IWUSR) ? "w" : "-";
    fileMode += (fileStat.st_mode & S_IXUSR) ? "x" : "-";
    fileMode += (fileStat.st_mode & S_IRGRP) ? "r" : "-";
    fileMode += (fileStat.st_mode & S_IWGRP) ? "w" : "-";
    fileMode += (fileStat.st_mode & S_IXGRP) ? "x" : "-";
    fileMode += (fileStat.st_mode & S_IROTH) ? "r" : "-";
    fileMode += (fileStat.st_mode & S_IWOTH) ? "w" : "-";
    fileMode += (fileStat.st_mode & S_IXOTH) ? "x" : "-";
    return fileMode;
}
/**
* @tc.name: IsUtf8_001
* @tc.desc: return true when string is valid UTF-8
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(JsonUtilsTest, IsUtf8_001, TestSize.Level0)
{
    std::string test = TwoByte() + ThreeByte() + FourByte() + "test";
    ASSERT_TRUE(JsonUtils::IsUtf8(test));
}

/**
* @tc.name: IsUtf8_002
* @tc.desc: return false when string is not valid UTF-8
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(JsonUtilsTest, IsUtf8_002, TestSize.Level0)
{
    std::string test = TwoByteWrong();
    ASSERT_FALSE(JsonUtils::IsUtf8(test));
}

/**
* @tc.name: IsUtf8_003
* @tc.desc: return false when string is not valid UTF-8
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(JsonUtilsTest, IsUtf8_003, TestSize.Level0)
{
    std::string test = ThreeByteWrong();
    ASSERT_FALSE(JsonUtils::IsUtf8(test));
}

/**
* @tc.name: IsUtf8_004
* @tc.desc: return false when string is not valid UTF-8
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(JsonUtilsTest, IsUtf8_004, TestSize.Level0)
{
    std::string test = ThreeByteWrong2();
    ASSERT_FALSE(JsonUtils::IsUtf8(test));
}

/**
* @tc.name: IsUtf8_005
* @tc.desc: return false when string is not valid UTF-8
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(JsonUtilsTest, IsUtf8_005, TestSize.Level0)
{
    std::string test = FourByteWrong();
    ASSERT_FALSE(JsonUtils::IsUtf8(test));
}

/**
* @tc.name: IsUtf8_006
* @tc.desc: return false when string is not valid UTF-8
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(JsonUtilsTest, IsUtf8_006, TestSize.Level0)
{
    std::string test = FourByteWrong2();
    ASSERT_FALSE(JsonUtils::IsUtf8(test));
}

/**
* @tc.name: WriteFileFromJson_001
* @tc.desc: return true when path is valid
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(JsonUtilsTest, WriteFileFromJson_001, TestSize.Level0)
{
    std::string path = "./" + TEST_FILE_NAME;
    json obj;
    obj["key"] = "value";
    ASSERT_TRUE(JsonUtils::WriteFileFromJson(path, obj));
}

/**
* @tc.name: ReadJsonFromFile_001
* @tc.desc: read json correctly when path is valid
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(JsonUtilsTest, ReadJsonFromFile_001, TestSize.Level0)
{
    std::string path = "./" + TEST_FILE_NAME;
    json obj;
    obj["key"] = "value";
    ASSERT_TRUE(JsonUtils::WriteFileFromJson(path, obj));
    std::pair<bool, nlohmann::json> result = JsonUtils::ReadJsonFromFile(path);
    ASSERT_TRUE(result.first);
    json resultObject = result.second;
    ASSERT_TRUE(resultObject.size() == 1);
    auto it = resultObject.find("key");
    ASSERT_TRUE(it != resultObject.end());
    ASSERT_TRUE(*it == "value");
}

/**
* @tc.name: IsFileExist_001
* @tc.desc: return true when file exit
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(JsonUtilsTest, IsFileExist_001, TestSize.Level0)
{
    std::string path = "./" + TEST_FILE_NAME;
    ASSERT_TRUE(JsonUtils::IsFileExist(path));
    RemoveTestFile(TEST_FILE_NAME);
    ASSERT_FALSE(JsonUtils::IsFileExist(path));
}

/**
* @tc.name: CopyFile_001
* @tc.desc: return true when file exit and path valid
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(JsonUtilsTest, CopyFile_001, TestSize.Level0)
{
    std::string src = "./" + TEST_FILE_NAME;
    json obj;
    obj["key"] = "value";
    ASSERT_TRUE(JsonUtils::WriteFileFromJson(src, obj));
    std::string des = COPY_FILE_NAME;
    ASSERT_TRUE(JsonUtils::CopyFile(src, des, true));
    struct stat fileStat;
    if (stat(COPY_FILE_NAME.c_str(), &fileStat) == 0) {
        ASSERT_EQ(FileMode(fileStat), ALL_USER_READ_MODE);
    }
    RemoveTestFile(COPY_FILE_NAME);
    ASSERT_TRUE(JsonUtils::CopyFile(src, des, false));
    if (stat(COPY_FILE_NAME.c_str(), &fileStat) == 0) {
        ASSERT_EQ(FileMode(fileStat), OWNER_ONLY_MODE);
    }
    RemoveTestFile(COPY_FILE_NAME);
}

/**
* @tc.name: GetJsonInt32Value_001
* @tc.desc: get int value when value type is int
* @tc.type: FUNC
* @tc.require: issueNumber
*/
HWTEST_F(JsonUtilsTest, GetJsonInt32Value_001, TestSize.Level0)
{
    std::string fileName = TEST_FILE_NAME;
    json obj;
    obj[fileName] = 1;
    ASSERT_EQ(JsonUtils::GetJsonInt32Value(obj, fileName, 0), 1);
    ASSERT_EQ(JsonUtils::GetJsonInt32Value(obj, fileName + " ", 0), 0);
    obj[fileName] = "1";
    ASSERT_EQ(JsonUtils::GetJsonInt32Value(obj, fileName, 0), 0);
}

}
}