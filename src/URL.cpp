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

#include <cstdlib>

#include <scraps/utility.h>

namespace scraps {

namespace {

std::tuple<std::string, std::string, uint16_t, std::string> Parse(const std::string& url) {
    const auto protocolEnd = url.find(":");
    std::string protocol   = protocolEnd == url.npos ? "http" : url.substr(0, protocolEnd);

    std::string host = "";
    uint16_t port = 0;

    auto resourceStart = protocolEnd + 1;

    if (protocolEnd == std::string::npos || url.find("//") == protocolEnd + 1) {
        const auto hostStart = protocolEnd == url.npos ? 0 : protocolEnd + 3;

        auto hostEnd = url.find('/', hostStart);

        if (protocol == "https") {
            port = 443;
        } else if (protocol == "http") {
            port = 80;
        }

        auto portSep = url.find(':', hostStart);

        if (portSep != url.npos) {
            const auto portStart  = portSep + 1;
            const auto portLength = hostEnd == url.npos ? url.size() - portStart : hostEnd - portStart;
            const auto portString = url.substr(portStart, portLength);
            port                  = static_cast<uint16_t>(std::atoi(portString.c_str()));
            hostEnd               = portSep;
        }

        auto hostLength = hostEnd == url.npos ? url.size() - hostStart : hostEnd - hostStart;
        host = url.substr(hostStart, hostLength);

        const auto portEnd = url.find('/', hostStart + hostLength);
        resourceStart = portEnd == url.npos ? url.size() : portEnd;
    }

    auto pound = url.find('#');
    auto resource = url.substr(resourceStart, pound == std::string::npos ? std::string::npos : (pound - resourceStart));

    return std::make_tuple(std::move(protocol), std::move(host), port, std::move(resource));
}

} // {anonymous} namespace

std::string URL::protocol() const { return std::get<0>(Parse(_url)); }

std::string URL::host() const { return std::get<1>(Parse(_url)); }

uint16_t URL::port() const { return std::get<2>(Parse(_url)); }

std::string URL::resource() const { return std::get<3>(Parse(_url)); }

std::string URL::path() const {
    auto res = resource();
    auto mark = res.find('?');
    return res.substr(0, mark == std::string::npos ? _url.find('#') : mark);
}

std::string URL::query() const {
    auto prefix = _url.find('?');
    auto pound = _url.find('#');
    return prefix == std::string::npos ? "" : _url.substr(prefix + 1, pound == std::string::npos ? std::string::npos : (pound - prefix - 1));
}

std::unordered_map<std::string, std::string> URL::ParseQuery(const std::string& query) {
    std::unordered_map<std::string, std::string> ret;

    size_t begin = 0;

    while (begin < query.size()) {
        size_t end = query.find('&', begin);

        std::string assignment = query.substr(begin, end == std::string::npos ? end : end - begin);

        if (!assignment.empty()) {
            size_t equals = assignment.find('=');
            if (equals == std::string::npos) {
                ret[URLDecode(assignment)]; // create but don't assign
            } else if (equals) {
                ret[URLDecode(assignment.substr(0, equals))] =
                    equals + 1 < assignment.size() ? URLDecode(assignment.substr(equals + 1)) : "";
            }
        }

        if (end == std::string::npos) {
            break;
        }

        begin = end + 1;
    }

    return ret;
}

} // namespace scraps
