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
#include "scraps/licenses.h"

namespace scraps {

namespace {
    namespace asio {
        #include <asio/license.c>
    }
    namespace sodium {
        #include <sodium/license.c>
    }
    namespace mbedtls {
        #include <mbedtls/license.c>
    }
    namespace curl {
        #include <curl/license.c>
    }
    namespace gsl {
        #include <gsl_license.c>
    }
    namespace mnmlstc {
        #include <core/license.c>
    }
    namespace fmtlib {
        #include <fmt/license.c>
    }
    namespace json11 {
        #include <json11/license.c>
    }
}

std::unordered_map<std::string, std::string> ThirdPartyLicenses() {
    static std::unordered_map<std::string, std::string> ret{{
        {"asio", {reinterpret_cast<char*>(asio::asio_LICENSE_1_0_txt), asio::asio_LICENSE_1_0_txt_len}},
        {"sodium", {reinterpret_cast<char*>(sodium::LICENSE), sodium::LICENSE_len}},
        {"mbedtls", {reinterpret_cast<char*>(mbedtls::LICENSE), mbedtls::LICENSE_len}},
        {"curl", {reinterpret_cast<char*>(curl::COPYING), curl::COPYING_len}},
        {"gsl", {reinterpret_cast<char*>(gsl::LICENSE), gsl::LICENSE_len}},
        {"mnmlstc", {reinterpret_cast<char*>(mnmlstc::License_rst), mnmlstc::License_rst_len}},
        {"fmtlib", {reinterpret_cast<char*>(fmtlib::LICENSE_rst), fmtlib::LICENSE_rst_len}},
        {"json11", {reinterpret_cast<char*>(json11::LICENSE_txt), json11::LICENSE_txt_len}},
    }};
    return ret;
}

} // namespace scraps
