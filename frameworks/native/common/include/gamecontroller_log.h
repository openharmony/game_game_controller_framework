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

#ifndef GAME_CONTROLLER_GAMECONTROLLER_LOG_H
#define GAME_CONTROLLER_GAMECONTROLLER_LOG_H

#include "hilog/log.h"

namespace OHOS {
namespace GameController {
#ifdef HILOGF
#undef HILOGF
#endif

#ifdef HILOGE
#undef HILOGE
#endif

#ifdef HILOGW
#undef HILOGW
#endif

#ifdef HILOGI
#undef HILOGI
#endif

#ifdef HILOGD
#undef HILOGD
#endif

#ifdef LOG_DOMAIN
#undef LOG_DOMAIN
#endif
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_DOMAIN 0xD004732
#define LOG_TAG "GameController"

#define FILE_NAME ""

#define HILOGE(fmt, ...) \
    HILOG_ERROR(LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt, FILE_NAME, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HILOGW(fmt, ...) \
    HILOG_WARN(LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt, FILE_NAME, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HILOGI(fmt, ...) \
    HILOG_INFO(LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt, FILE_NAME, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HILOGD(fmt, ...) \
    HILOG_DEBUG(LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt, FILE_NAME, __FUNCTION__, __LINE__, ##__VA_ARGS__)
}
}
#endif //GAME_CONTROLLER_GAMECONTROLLER_LOG_H
