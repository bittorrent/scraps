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
#include "scraps/base64.h"

namespace scraps {

std::string Base64Decode(stdts::string_view data) {
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

    assert(data.length() >= 4 && data.length() % 4 == 0);

    std::string decoded;
    decoded.reserve(data.length() / 4 * 3);

    for (size_t i = 0; i < data.length(); i += 4) {
        auto temp = (decodeMap[data[i    ] - '+'] << 18) +
                    (decodeMap[data[i + 1] - '+'] << 12) +
                    (decodeMap[data[i + 2] - '+'] << 6 ) +
                    (decodeMap[data[i + 3] - '+']      );
        decoded.append({static_cast<char>(temp >> 16),
                        static_cast<char>(temp >> 8),
                        static_cast<char>(temp)});
    }

    if (data[data.length() - 1] == '=') { decoded.pop_back(); }
    if (data[data.length() - 2] == '=') { decoded.pop_back(); }

    return decoded;
}

std::string Base64Encode(stdts::string_view data) {
    static constexpr auto base64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encoded;
    encoded.reserve(((data.length() / 3) + ((data.length() % 3) > 0)) * 4);

    auto c = data.data();
    uint_fast32_t temp = 0;
    for (size_t i = 0; i < (data.length() / 3); ++i) {
        temp  = *c++ << 16;
        temp += *c++ << 8;
        temp += *c++;
        encoded.append({base64chars[(temp & 0b111111000000000000000000) >> 18],
                        base64chars[(temp & 0b000000111111000000000000) >> 12],
                        base64chars[(temp & 0b000000000000111111000000) >> 6 ],
                        base64chars[(temp & 0b000000000000000000111111)      ]});
    }

    switch (data.length() % 3) {
    case 1:
        temp = *c++ << 16;
        encoded.append({base64chars[(temp & 0b111111000000000000000000) >> 18],
                        base64chars[(temp & 0b000000111111000000000000) >> 12],
                        '=',
                        '='});
        break;
    case 2:
        temp  = *c++ << 16;
        temp += *c++ << 8;
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

} // namespace scraps
