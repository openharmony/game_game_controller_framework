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

#ifndef GAME_CONTROLLER_GAMECONTROLLER_ERRORS_H
#define GAME_CONTROLLER_GAMECONTROLLER_ERRORS_H
namespace OHOS {
namespace GameController {
const int GAME_CONTROLLER_SUCCESS = 0;

enum {
    GAME_ERR_FAIL = 32200001,
    GAME_ERR_ARGUMENT_NULL,
    GAME_ERR_IPC_CONNECT_STUB_FAIL,
    GAME_ERR_TIMEOUT,
    GAME_ERR_CALL_MULTI_INPUT_FAIL,
    GAME_ERR_ENOMEM,
    GAME_ERR_BUNDLE_NAME_INVALID,
    GAME_ERR_ARRAY_MAXSIZE,
    GAME_ERR_NO_SYS_PERMISSIONS,
    GAME_ERR_ARGUMENT_INVALID,
};
}
}
#endif //GAME_CONTROLLER_GAMECONTROLLER_ERRORS_H
