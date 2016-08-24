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
