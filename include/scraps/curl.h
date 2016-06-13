#pragma once

#include "scraps/assert.h"
#include "scraps/openssl.h"

#include <curl/curl.h>

namespace scraps {

#ifdef OPENSSL_THREADS

void InitCURLThreadSafety();
bool CURLIsThreadSafe();

#else

inline bool CURLIsThreadSafe() { return false; }

#endif

} // namespace scraps
