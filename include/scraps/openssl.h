#pragma once

namespace scraps {

#define OPENSSL_THREAD_DEFINES
#include <openssl/opensslconf.h>
#ifdef OPENSSL_THREADS

void InitOpenSSLThreadSafety();
bool OpenSSLIsThreadSafe();

#else

inline bool OpenSSLIsThreadSafe() { return false; }

#endif

} // namespace scraps
