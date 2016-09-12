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

using namespace std::literals;

namespace detail {
std::unordered_map<std::string, std::vector<Assertion*>>* AssertionsMap() {
    static std::unique_ptr<std::unordered_map<std::string, std::vector<Assertion*>>> assertions = nullptr;

    if (!assertions) {
        assertions = std::make_unique<std::unordered_map<std::string, std::vector<Assertion*>>>();
    }

    return assertions.get();
}

std::vector<Assertion*> GetAssertions(const std::string& name) {
    auto it = detail::AssertionsMap()->find(name);
    if (it == detail::AssertionsMap()->end()) {
        return {};
    }
    return it->second;
}
} // namespace detail

bool ComplexityAsserter::Success() {
    for (auto& kv : *detail::AssertionsMap()) {
        for (auto& a : kv.second) {
            if (a->didRun && !a->success) {
                return false;
            }
        }
    }
    return true;
}

void ComplexityAsserter::ReportRuns(const std::vector<Run>& report) {
    // provide normal statistics before showing complexity failure analysis.
    Base::ReportRuns(report);

    auto matchesBigOName = [](auto name) {
        const auto suffix = "_BigO"s;
        return name.size() > suffix.size() && name.substr(name.size() - suffix.size(), suffix.size()) == suffix;
    };

    auto stripBigO = [](auto name) {
        const auto suffix = "_BigO"s;
        return name.substr(0, name.size() - suffix.size());
    };

    auto printResult = [](auto name, auto assertion) {
        if (assertion.success) {
            std::cout << name << " [ OK ] " << std::endl;
        } else {
            std::cout << name << " [ FAIL ] (" << assertion.file << ":" << assertion.line << ")" << std::endl;
        }
    };

    for (auto& r : report) {
        if (matchesBigOName(r.benchmark_name)) {
            auto testName = stripBigO(r.benchmark_name);
            for (auto& a : detail::GetAssertions(testName)) {
                (*a)(r);
                printResult(testName, *a);
            }
        }
    }
}

std::unique_ptr<Assertion> ComplexityAsserter::registerAssertion(std::string benchmarkName, const char* file, size_t line, std::function<bool(benchmark::BenchmarkReporter::Run)> func) {
    auto ret = std::make_unique<Assertion>(file, line, std::move(func));
    (*detail::AssertionsMap())[benchmarkName].push_back(ret.get());
    return ret;
}
