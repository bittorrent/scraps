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
#include <scraps/utility.h>

#include <scraps/format.h>

#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#if !SCRAPS_WINDOWS
#include <termios.h>
#include <unistd.h>
#endif

#if __clang__ || __GNUC__
#include <cxxabi.h>
#endif

#include <fcntl.h>

#if SCRAPS_MACOS
// for sysctlbyname
#include <sys/sysctl.h>
#endif

namespace scraps {

std::string JSONEscape(const char* str) {
    std::string ret;
    while (*str) {
        if (*str < 0x20) {
            ret += "\\u";
            ret += Formatf("%04X", (unsigned int)*(uint8_t*)str);
        } else if (*str == '"' || *str == '\\') {
            ret += '\\';
            ret += *str;
        } else {
            ret += *str;
        }
        ++str;
    }
    return ret;
}

std::string URLEncode(const char* str) {
    std::ostringstream ret;

    while (char c = *str) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.') {
            ret << c;
        } else if (c == ' ') {
            ret << '+';
        } else {
            ret << '%' << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (int)static_cast<unsigned char>(c);
        }
        ++str;
    }

    return ret.str();
}

std::string URLDecode(const char* str) {
    std::ostringstream ret;

    while (char c = *str) {
        if (c == '+') {
            ret << ' ';
        } else if (c == '%' && str[1] && str[2]) {
            char tmp[3];
            tmp[0] = *++str;
            tmp[1] = *++str;
            tmp[2] = '\0';
            ret << (unsigned char)strtoul(tmp, nullptr, 16);
        } else {
            ret << c;
        }
        ++str;
    }

    return ret.str();
}

std::tuple<std::string, uint16_t> ParseAddressAndPort(const std::string& host,
                                                      uint16_t defaultPort) {
    auto sep = host.find(':');
    if (sep == host.npos) {
        return std::make_tuple(host, defaultPort);
    }

    const auto address = host.substr(0, sep);
    const auto port = static_cast<uint16_t>(std::atoi(host.substr(sep + 1).c_str()));
    return std::make_tuple(address, port);
}

size_t PhysicalMemory() {
#if SCRAPS_MACOS
    uint64_t mem = 0;
    size_t len = sizeof(mem);
    return sysctlbyname("hw.memsize", &mem, &len, NULL, 0) == 0 ? mem : 0;
#else
    auto pages = sysconf(_SC_PHYS_PAGES);
    auto pageSize = sysconf(_SC_PAGE_SIZE);
    return (pages > 0 && pageSize > 0) ? pages * pageSize : 0;
#endif
}

stdts::optional<std::vector<Byte>> BytesFromFile(const std::string& path) {
    std::ifstream dataFile{path, std::ifstream::ate | std::ios::binary};
    const auto fileSize = dataFile.tellg();

    if (dataFile.bad() || fileSize <= 0) {
        return {};
    }

    std::vector<Byte> buf;
    buf.resize(fileSize);
    dataFile.seekg(0, std::ios::beg);
    dataFile.read(reinterpret_cast<char*>(buf.data()), fileSize);

    if (dataFile.bad()) {
        return {};
    }

    return buf;
}

bool SetBlocking(int fd, bool blocking) {
#ifdef _WIN32
    unsigned long arg = blocking ? 1 : 0;
    return !ioctlsocket(fd, FIONBIO, &arg);
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return false;
    }
    return !fcntl(fd, F_SETFL, blocking ? flags & ~O_NONBLOCK : flags | O_NONBLOCK);
#endif
}

std::string Demangle(const char* mangled) {
#if __clang__ || __GNUC__
    struct FreeDeleter { void operator()(char* p) const { std::free(p); } };
    int status = 0;
    auto demangled = std::unique_ptr<char, FreeDeleter>{abi::__cxa_demangle(mangled, nullptr, nullptr, &status)};
    if (status == 0) {
        return demangled.get();
    }
#endif
    return mangled;
}

} // namespace scraps
