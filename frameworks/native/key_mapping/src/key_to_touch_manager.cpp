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

#include "key_to_touch_manager.h"

namespace OHOS {
namespace GameController {

KeyToTouchManager::KeyToTouchManager()
{
    handleQueue_ = std::make_unique<ffrt::queue>("KeyToTouchHandleQueue",
                                                 ffrt::queue_attr().qos(ffrt::qos_default));
}

KeyToTouchManager::~KeyToTouchManager()
{

}

bool KeyToTouchManager::DispatchKeyEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    return false;
}

bool KeyToTouchManager::DispatchPointerEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    return false;
}

void KeyToTouchManager::UpdateTemplateConfig(const DeviceInfo &deviceInfo,
                                             const std::vector<KeyToTouchMappingInfo> &mappingInfos)
{

}

void KeyToTouchManager::UpdateWindowInfo(const WindowInfoEntity &windowInfoEntity)
{

}

bool KeyToTouchManager::HandleKeyEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent, const DeviceInfo &deviceInfo)
{
    return false;
}

bool KeyToTouchManager::HandlePointerEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent,
                                           const DeviceInfo &deviceInfo)
{
    return false;
}

void KeyToTouchManager::HandleTemplateConfig(const DeviceInfo &deviceInfo,
                                             const std::vector<KeyToTouchMappingInfo> &mappingInfos)
{

}

void KeyToTouchManager::HandleWindowInfo(const WindowInfoEntity &windowInfoEntity)
{

}
}
}