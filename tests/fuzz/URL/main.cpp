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
#include <scraps/URL.h>

#include <fstream>
#include <streambuf>
#include <string>
#include <cstdio>

int main(int argc, const char* argv[]) {
    if (argc < 2) { return 1; }

    std::ifstream f(argv[1]);
    std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    scraps::URL url{str};
    printf("%s\n", url.str().c_str());
    printf("%s\n", url.protocol().c_str());
    printf("%s\n", url.host().c_str());
    printf("%s\n", url.resource().c_str());
    printf("%s\n", url.path().c_str());
    printf("%s\n", url.query().c_str());
    printf("%hu\n", url.port());
    for (auto& kv : scraps::URL::ParseQuery(url.query())) {
        printf("%s=%s\n", kv.first.c_str(), kv.second.c_str());
    }

    return 0;
}
