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
#include "complexity.h"

#if SCRAPS_APPLE
#include <scraps/apple/SHA256.h>
#endif

#include <scraps/random.h>
#include <scraps/sodium/SHA256.h>
#include <scraps/SHA256.h>

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

template <typename SHA256Type>
void SHA256Complexity(benchmark::State& state) {
    SHA256Type sha256;
    std::array<unsigned char, SHA256Type::kHashSize> result;
    while (state.KeepRunning()) {
        state.PauseTiming();
        auto bytes = GetRandomTestBytes(state.range(0));
        memset(result.data(), 0, result.size());
        sha256.reset();
        state.ResumeTiming();
        sha256.update(bytes.data(), bytes.size());
        sha256.finish(result.data());
        benchmark::DoNotOptimize(&result);
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(state.range(0));
}

void SHA256ComplexityAllInOne(benchmark::State& state) {
    while (state.KeepRunning()) {
        state.PauseTiming();
        auto bytes = GetRandomTestBytes(state.range(0));
        state.ResumeTiming();
        auto result = GetSHA256(gsl::as_span(bytes));
        benchmark::DoNotOptimize(&result);
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(state.range(0));
}

#if SCRAPS_APPLE

void SHA256ComplexityApple(benchmark::State& state) {
    SHA256Complexity<scraps::apple::SHA256>(state);
}
BENCHMARK(SHA256ComplexityApple)->RangeMultiplier(4)->Range(256, 1<<15)->Complexity();
EXPECT_COMPLEXITY_LE(SHA256ComplexityApple, benchmark::oN);

#endif // SCRAPS_APPLE

void SHA256ComplexitySodium(benchmark::State& state) {
    SHA256Complexity<scraps::sodium::SHA256>(state);
}

BENCHMARK(SHA256ComplexityAllInOne)->RangeMultiplier(4)->Range(256, 1<<15)->Complexity();
BENCHMARK(SHA256ComplexitySodium)->RangeMultiplier(4)->Range(256, 1<<15)->Complexity();

EXPECT_COMPLEXITY_LE(SHA256ComplexityAllInOne, benchmark::oN);
EXPECT_COMPLEXITY_LE(SHA256ComplexitySodium, benchmark::oN);
