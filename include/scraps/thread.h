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
