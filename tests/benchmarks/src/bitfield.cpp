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

#include "scraps/bitfield.h"
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

static void BitfieldEncodeComplexity(benchmark::State& state) {
    while (state.KeepRunning()) {
        state.PauseTiming();
        auto bytes = GetRandomTestBytes(state.range(0));
        state.ResumeTiming();
        auto result = BitfieldEncode(bytes);
        benchmark::DoNotOptimize(&result);
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BitfieldEncodeComplexity)->RangeMultiplier(4)->Range(256, 1<<12)->Complexity();
EXPECT_COMPLEXITY_LE(BitfieldEncodeComplexity, benchmark::oN);

static void BitfieldDecodeComplexity(benchmark::State& state) {
    while (state.KeepRunning()) {
        state.PauseTiming();
        auto bytes = GetRandomTestBytes(state.range(0));
        auto encoded = BitfieldEncode(bytes);
        state.ResumeTiming();
        auto result = BitfieldDecode(encoded.c_str(), encoded.size());
        benchmark::DoNotOptimize(&result);
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BitfieldDecodeComplexity)->RangeMultiplier(4)->Range(256, 1<<12)->Complexity();
EXPECT_COMPLEXITY_LE(BitfieldDecodeComplexity, benchmark::oN);
