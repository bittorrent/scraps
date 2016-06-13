#include "scraps/curl.h"

#ifdef OPENSSL_THREADS

namespace scraps {

namespace {
bool _gIsCURLThreadSafe = false;
}

void InitCURLThreadSafety() {
    if (CURLIsThreadSafe()) { return; }

    InitOpenSSLThreadSafety();
    auto result = curl_global_init(CURL_GLOBAL_ALL);
    SCRAPS_ASSERT(result == CURLE_OK);
    _gIsCURLThreadSafe = true;
}

bool CURLIsThreadSafe() {
    return _gIsCURLThreadSafe;
}

} // namespace scraps

#endif
