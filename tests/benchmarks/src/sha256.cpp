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

#include "scraps/SHA256.h"
#include "scraps/detail/SHA256Sodium.h"
#include "scraps/random.h"
#include "complexity.h"

#include <benchmark/benchmark.h>
#include <iostream>
#include <random>

using namespace scraps;

namespace {
auto GetRandomTestBytes(size_t numBytes) {
    static std::random_device rd;
    static auto rng = [&]{
        std::mt19937 rng;
        rng.seed(rd());
        return rng;
    }();

    return RandomBytes(numBytes, rng);
}
} // anonymous namespace

template <typename T>
static void SHA256Complexity(benchmark::State& state) {
    while (state.KeepRunning()) {
        state.PauseTiming();
        auto bytes = GetRandomTestBytes(state.range(0));
        std::array<unsigned char, scraps::SHA256::kHashSize> result;
        state.ResumeTiming();
        T sha256;
        sha256.update(bytes.data(), bytes.size());
        sha256.finish(result.data());
        benchmark::DoNotOptimize(&result);
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(state.range(0));
}

static void SHA256ComplexityNative(benchmark::State& state) {
    SHA256Complexity<scraps::SHA256>(state);
}

static void SHA256ComplexitySodium(benchmark::State& state) {
    SHA256Complexity<scraps::detail::SHA256Sodium>(state);
}

BENCHMARK(SHA256ComplexitySodium)->RangeMultiplier(4)->Range(256, 1<<15)->Complexity();

BENCHMARK(SHA256ComplexityNative)->RangeMultiplier(4)->Range(256, 1<<15)->Complexity();
