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
#include "input_event_client.h"
#include "input_event_callback.h"

namespace OHOS {
namespace GameController {

void InputEventClient::RegisterGamePadButtonEventCallback(const ApiTypeEnum apiTypeEnum,
                                                          const GamePadButtonTypeEnum gamePadButtonTypeEnum,
                                                          const std::shared_ptr<GamePadButtonCallbackBase> &callback)
{
    DelayedSingleton<InputEventCallback>::GetInstance()->RegisterGamePadButtonEventCallback(apiTypeEnum,
                                                                                            gamePadButtonTypeEnum,
                                                                                            callback);
}

void InputEventClient::UnRegisterGamePadButtonEventCallback(const ApiTypeEnum apiTypeEnum,
                                                            const GamePadButtonTypeEnum gamePadButtonTypeEnum)
{
    DelayedSingleton<InputEventCallback>::GetInstance()->UnRegisterGamePadButtonEventCallback(apiTypeEnum,
                                                                                              gamePadButtonTypeEnum);
}

void InputEventClient::RegisterGamePadAxisEventCallback(const ApiTypeEnum apiTypeEnum,
                                                        const GamePadAxisSourceTypeEnum gamePadAxisTypeEnum,
                                                        const std::shared_ptr<GamePadAxisCallbackBase> &callback)
{
    DelayedSingleton<InputEventCallback>::GetInstance()->RegisterGamePadAxisEventCallback(apiTypeEnum,
                                                                                          gamePadAxisTypeEnum,
                                                                                          callback);
}

void InputEventClient::UnRegisterGamePadAxisEventCallback(const ApiTypeEnum apiTypeEnum,
                                                          const GamePadAxisSourceTypeEnum gamePadAxisTypeEnum)
{
    DelayedSingleton<InputEventCallback>::GetInstance()->UnRegisterGamePadAxisEventCallback(apiTypeEnum,
                                                                                            gamePadAxisTypeEnum);
}
}
}