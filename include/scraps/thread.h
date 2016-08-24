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
#pragma once

#include "scraps/config.h"

#include <condition_variable>
#include <mutex>
#include <thread>

#if SCRAPS_MAC_OS_X || SCRAPS_IOS
#include <pthread.h>
#elif SCRAPS_LINUX
#include <sys/prctl.h>
#endif

namespace scraps {

inline void SetThreadName(const char* name) {
#if SCRAPS_MAC_OS_X || SCRAPS_IOS
    pthread_setname_np(name);
#elif SCRAPS_LINUX
    prctl(PR_SET_NAME, name, 0, 0, 0);
#endif
}

inline void SetThreadName(const std::string& name) { SetThreadName(name.c_str()); }

} // namespace scraps
