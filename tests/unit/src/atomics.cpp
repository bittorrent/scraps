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
#include "gtest.h"

#include <atomic>
#include <limits>

/**
 * When linking with -latomic on Android with Clang, it appears that
 * atomics are horribly broken if the linker uses the default arm
 * libraries. When using armv7-a, everything should work fine. This
 * test should ensure that we aren't using a horribly broken atomics
 * implementation.
 *
 * By sublcassing GTest's Environment, we ensure that this series of
 * tests are run before any of the standard unit tests. This is
 * necessary in case of a failure as a failure here will likely
 * cause a number of other potentially-hard-to-diagnose test
 * failures.
 */
class SaneAtomics : public ::testing::Environment {
public:
    virtual void SetUp() {
        _assertAtomics<uint8_t>();
        _assertAtomics<uint16_t>();
        _assertAtomics<uint32_t>();
        _assertAtomics<uint64_t>();
    }

private:
    template <typename AtomicType>
    void _assertAtomics() {
        constexpr auto max = std::numeric_limits<AtomicType>::max();

        std::atomic<AtomicType> v{0x00u};
        ASSERT_EQ(v.load(), 0x00u);
        v = max;
        ASSERT_EQ(v.load(), max);
        v = 0x00u;
        ASSERT_EQ(v.load(), 0x00u);
    }
};

::testing::Environment* const gSaneAtomics = ::testing::AddGlobalTestEnvironment(new SaneAtomics);
