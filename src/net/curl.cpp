/**
* Copyright 2016 BitTorrent Inc.
*
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
#include "scraps/net/curl.h"

namespace scraps {
namespace net {

namespace {
bool _gCURLIsInitialized = false;
}

void InitializeCURL() {
    if (CURLIsInitialized()) { return; }

    auto result = curl_global_init(CURL_GLOBAL_ALL);
    SCRAPS_ASSERT(result == CURLE_OK);
    _gCURLIsInitialized = true;
}

bool CURLIsInitialized() {
    return _gCURLIsInitialized;
}

}} // namespace scraps::net

