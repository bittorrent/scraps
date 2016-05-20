#include "scraps/utility.h"

#include "scraps/format.h"

#include <ctype.h>
#include <iomanip>
#include <iostream>
#include <sstream>

#if !SCRAPS_WINDOWS
#include <termios.h>
#include <unistd.h>
#endif

namespace scraps {

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

std::string Base64Decode(const char* data, size_t length) {
    // maps the ascii value of a base64 encoded char minus '+' (the lowest ascii value of the base64 encoded char set) to
    // the index in the base64 character set.
    static constexpr uint_fast32_t decodeMap[] = {
        62, // +
        0, 0, 0,
        63, // /
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // 0-9
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // A-Z
        0, 0, 0,
        0, // =
        0, 0,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 // a-z
    };

    assert(length >= 4 && length % 4 == 0);

    std::string decoded;
    decoded.reserve(length / 4 * 3);

    for (size_t i = 0; i < length; i += 4) {
        auto temp = (decodeMap[data[i    ] - '+'] << 18) +
                    (decodeMap[data[i + 1] - '+'] << 12) +
                    (decodeMap[data[i + 2] - '+'] << 6 ) +
                    (decodeMap[data[i + 3] - '+']      );
        decoded.append({static_cast<char>(temp >> 16),
                        static_cast<char>(temp >> 8),
                        static_cast<char>(temp)});
    }

    if (data[length - 1] == '=') { decoded.pop_back(); }
    if (data[length - 2] == '=') { decoded.pop_back(); }

    return decoded;
}

std::string Base64Encode(const char* data, size_t length) {
    static constexpr auto base64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encoded;
    encoded.reserve(((length / 3) + ((length % 3) > 0)) * 4);

    uint_fast32_t temp = 0;
    for (size_t i = 0; i < (length / 3); ++i) {
        temp  = *data++ << 16;
        temp += *data++ << 8;
        temp += *data++;
        encoded.append({base64chars[(temp & 0b111111000000000000000000) >> 18],
                        base64chars[(temp & 0b000000111111000000000000) >> 12],
                        base64chars[(temp & 0b000000000000111111000000) >> 6 ],
                        base64chars[(temp & 0b000000000000000000111111)      ]});
    }

    switch (length % 3) {
    case 1:
        temp = *data++ << 16;
        encoded.append({base64chars[(temp & 0b111111000000000000000000) >> 18],
                        base64chars[(temp & 0b000000111111000000000000) >> 12],
                        '=',
                        '='});
        break;
    case 2:
        temp  = *data++ << 16;
        temp += *data++ << 8;
        encoded.append({base64chars[(temp & 0b111111000000000000000000) >> 18],
                        base64chars[(temp & 0b000000111111000000000000) >> 12],
                        base64chars[(temp & 0b000000000000111111000000) >> 6 ],
                        '='});
        break;
    default:
        break;
    }

    return encoded;
}

std::string Basename(const std::string& path) {
    auto folder = path.rfind('/');
    #if SCRAPS_WINDOWS
    if (folder == path.npos) {
        folder = path.rfind('\\');
    }
    #endif
    if (folder != path.npos) {
        return path.substr(folder+1);
    }

    return path;
}

std::string Dirname(const std::string& path) {
    auto folder = path.rfind('/');
    #if SCRAPS_WINDOWS
    if (folder == path.npos) {
        folder = path.rfind('\\');
    }
    #endif
    if (folder != path.npos) {
        return path.substr(0, folder);
    }

    return path;
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

std::string GetPasswordFromStdin() {
#ifdef WIN32
    HANDLE stdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(stdin, &mode);
    SetConsoleMode(stdin, mode & (~ENABLE_ECHO_INPUT));
#else
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif

    std::string password;
    std::cout << "Password: " << std::flush;
    std::getline(std::cin, password);
    return password;
}

size_t PhysicalMemory() {
    auto pages = sysconf(_SC_PHYS_PAGES);
    auto pageSize = sysconf(_SC_PAGE_SIZE);
    return (pages > 0 && pageSize > 0) ? pages * pageSize : 0;
}

} // namespace scraps
