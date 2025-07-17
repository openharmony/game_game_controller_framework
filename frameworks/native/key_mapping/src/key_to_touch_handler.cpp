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

#include <window_input_intercept_client.h>
#include "key_to_touch_handler.h"
#include "gamecontroller_log.h"

namespace OHOS {
namespace GameController {
namespace {
const int32_t DEVICE_ID = 3;
}

void BaseKeyToTouchHandler::BuildAndSendPointerEvent(InputToTouchContext &context,
                                                     const TouchEntity &touchEntity)
{
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    if (pointerEvent == nullptr) {
        HILOGE("Create PointerEvent failed.");
        return;
    }
    PointerEvent::PointerItem pointerItem = BuildPointerItem(context, touchEntity);
    if (touchEntity.pointerAction == PointerEvent::POINTER_ACTION_UP) {
        if (context.pointerItems.find(touchEntity.pointerId) != context.pointerItems.end()) {
            context.pointerItems.erase(touchEntity.pointerId);
        }
        pointerEvent->AddPointerItem(pointerItem);
    } else {
        context.pointerItems[touchEntity.pointerId] = pointerItem;
    }
    for (auto &pair: context.pointerItems) {
        pointerEvent->AddPointerItem(pair.second);
    }
    pointerEvent->SetId(std::numeric_limits<int32_t>::max());
    pointerEvent->SetPointerAction(touchEntity.pointerAction);
    pointerEvent->SetPointerId(touchEntity.pointerId);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    HILOGD("pointer is [%{public}s].", pointerEvent->ToString().c_str());
    Rosen::WindowInputInterceptClient::SendInputEvent(pointerEvent);
}

PointerEvent::PointerItem BaseKeyToTouchHandler::BuildPointerItem(InputToTouchContext &context,
                                                                  const TouchEntity &touchEntity)
{
    PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(touchEntity.pointerId);
    pointerItem.SetOriginPointerId(touchEntity.pointerId);
    pointerItem.SetDisplayX(touchEntity.xValue);
    pointerItem.SetDisplayXPos(touchEntity.xValue);
    pointerItem.SetWindowX(touchEntity.xValue);
    pointerItem.SetWindowXPos(touchEntity.xValue);
    pointerItem.SetDisplayY(touchEntity.yValue);
    pointerItem.SetDisplayYPos(touchEntity.yValue);
    pointerItem.SetWindowY(touchEntity.yValue);
    pointerItem.SetWindowYPos(touchEntity.yValue);
    pointerItem.SetDeviceId(DEVICE_ID);
    pointerItem.SetDownTime(touchEntity.actionTime);
    return pointerItem;
}
}
}