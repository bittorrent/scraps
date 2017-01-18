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
#include <scraps/net/HTTPRequest.h>
#include <scraps/utility.h>

#include <fstream>
#include <streambuf>
#include <string>
#include <cstdio>

int main(int argc, const char* argv[]) {
    if (argc < 2) { return 1; }

    std::ifstream f(argv[1]);
    std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    std::vector<std::string> lines;
    scraps::Split(str.begin(), str.end(), std::back_inserter(lines), '\n');
    auto c = scraps::net::detail::CookiesFromHTTPResponseHeaders(lines);
    return c.size();
}
