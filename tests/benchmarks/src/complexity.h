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

#pragma once

#include <benchmark/benchmark.h>

#include <iostream>
#include <string>
#include <unordered_map>

class Assertion {
public:
    template <typename F>
    Assertion(std::string file, size_t line, F&& f)
        : file{std::move(file)}
        , line{line}
        , _f{std::move(f)}
    {}

    void operator()(benchmark::BenchmarkReporter::Run r) {
        didRun = true;
        success = _f(r);
    }

    const std::string file;
    const size_t line = 0;
    bool success = false;
    bool didRun = false;

private:
    std::function<bool(benchmark::BenchmarkReporter::Run)> _f;
};

class ComplexityAsserter : public benchmark::ConsoleReporter {
public:
    using Base = benchmark::ConsoleReporter;
    using Base::Base;

    void ReportRuns(const std::vector<Run>& report) override;

    static bool Success();

    // use the provided macros below instead of calling this regularly
    static std::unique_ptr<Assertion> registerAssertion(std::string benchmarkName, const char* file, size_t line, std::function<bool(benchmark::BenchmarkReporter::Run)> func);
};

constexpr auto BigORank(benchmark::BigO bigO) {
    switch(bigO) {
    case benchmark::o1:        return 1;
    case benchmark::oLogN:     return 2;
    case benchmark::oN:        return 3;
    case benchmark::oNLogN:    return 4;
    case benchmark::oNSquared: return 5;
    case benchmark::oNCubed:   return 6;
    case benchmark::oAuto:   // fallthrough
    case benchmark::oLambda: // fallthrough
    case benchmark::oNone:   // fallthrough
    default:
        return 9;
    }
}

#define CONCAT_(x, y) x ## y
#define CONCAT(x, y) CONCAT_(x, y)

// For a full list of complexity measurements, see benchmark_api.h from google benchmark
#define EXPECT_COMPLEXITY_LT(benchmark_name, complexity_enum)                               \
    static std::unique_ptr<Assertion> CONCAT(ComplexityAssertion_, __COUNTER__) =           \
    ComplexityAsserter::registerAssertion(#benchmark_name, __FILE__, __LINE__, [](auto r) { \
        return BigORank(r.complexity) <  BigORank(complexity_enum);                         \
    });

#define EXPECT_COMPLEXITY_LE(benchmark_name, complexity_enum)                               \
    static std::unique_ptr<Assertion> CONCAT(ComplexityAssertion_, __COUNTER__) =           \
    ComplexityAsserter::registerAssertion(#benchmark_name, __FILE__, __LINE__, [](auto r) { \
        return BigORank(r.complexity) <= BigORank(complexity_enum);                         \
    });

#define EXPECT_COMPLEXITY_EQ(benchmark_name, complexity_enum)                               \
    static std::unique_ptr<Assertion> CONCAT(ComplexityAssertion_, __COUNTER__) =           \
    ComplexityAsserter::registerAssertion(#benchmark_name, __FILE__, __LINE__, [](auto r) { \
        return BigORank(r.complexity) == BigORank(complexity_enum);                         \
    });

#define EXPECT_COMPLEXITY_GT(benchmark_name, complexity_enum)                               \
    static std::unique_ptr<Assertion> CONCAT(ComplexityAssertion_, __COUNTER__) =           \
    ComplexityAsserter::registerAssertion(#benchmark_name, __FILE__, __LINE__, [](auto r) { \
        return BigORank(r.complexity) >  BigORank(complexity_enum);                         \
    });

#define EXPECT_COMPLEXITY_GE(benchmark_name, complexity_enum)                               \
    static std::unique_ptr<Assertion> CONCAT(ComplexityAssertion_, __COUNTER__) =           \
    ComplexityAsserter::registerAssertion(#benchmark_name, __FILE__, __LINE__, [](auto r) { \
        return BigORank(r.complexity) >= BigORank(complexity_enum);                         \
    });

#define EXPECT_RUN(benchmark_name, functor)                                                 \
    static std::unique_ptr<Assertion> CONCAT(ComplexityAssertion_, __COUNTER__) =           \
    ComplexityAsserter::registerAssertion(#benchmark_name, __FILE__, __LINE__, functor);

#define COMPLEXITY_ASSERTION_MAIN()                \
int main(int argc, char** argv) {                  \
    benchmark::Initialize(&argc, argv);            \
    ComplexityAsserter rep;                        \
    benchmark::RunSpecifiedBenchmarks(&rep);       \
    return ComplexityAsserter::Success() ? 0 : 1;  \
}
