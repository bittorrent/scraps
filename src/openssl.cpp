#include "scraps/openssl.h"

#ifdef OPENSSL_THREADS

#include "scraps/assert.h"

#include <openssl/conf.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>

#include <vector>
#include <mutex>
#include <thread>
#include <memory>

namespace scraps {
namespace {

std::vector<std::unique_ptr<std::mutex>> _gOpenSSLMutexes;

extern "C" void OpenSSLLockCallback(int mode, int type, const char* /* file */, int /* line */) {
    SCRAPS_ASSERT(type < _gOpenSSLMutexes.size());
    if (mode & CRYPTO_LOCK) {
        _gOpenSSLMutexes[type]->lock();
    } else {
        _gOpenSSLMutexes[type]->unlock();
    }
}

extern "C" unsigned long OpenSSLThreadIdCallback() {
    return std::hash<std::thread::id>()(std::this_thread::get_id());
}

} // anonymous namespace

void InitOpenSSLThreadSafety() {
    if (OpenSSLIsThreadSafe()) { return; }

    ERR_load_CRYPTO_strings();
    OPENSSL_add_all_algorithms_noconf();

    _gOpenSSLMutexes.resize(CRYPTO_num_locks());
    for (auto& mutex : _gOpenSSLMutexes) {
        mutex = std::make_unique<std::mutex>();
    }

    CRYPTO_set_id_callback(&OpenSSLThreadIdCallback);
    CRYPTO_set_locking_callback(&OpenSSLLockCallback);

    OPENSSL_no_config();
}

bool OpenSSLIsThreadSafe() {
    return CRYPTO_get_locking_callback() && CRYPTO_get_id_callback();
}

} // namespace scraps

#endif // OPENSSL_THREADS
