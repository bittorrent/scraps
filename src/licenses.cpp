#include "scraps/licenses.h"

namespace scraps {

namespace {
    namespace boost {
        #include <boost/license.c>
    }
    namespace sodium {
        #include <sodium/license.c>
    }
    namespace openssl {
        #include <openssl/license.c>
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
}

std::unordered_map<std::string, std::string> ThirdPartyLicenses() {
    static std::unordered_map<std::string, std::string> ret{{
        {"boost", {reinterpret_cast<char*>(boost::LICENSE_1_0_txt), boost::LICENSE_1_0_txt_len}},
        {"sodium", {reinterpret_cast<char*>(sodium::LICENSE), sodium::LICENSE_len}},
        {"openssl", {reinterpret_cast<char*>(openssl::LICENSE), openssl::LICENSE_len}},
        {"curl", {reinterpret_cast<char*>(curl::COPYING), curl::COPYING_len}},
        {"gsl", {reinterpret_cast<char*>(gsl::LICENSE), gsl::LICENSE_len}},
        {"mnmlstc", {reinterpret_cast<char*>(mnmlstc::License_rst), mnmlstc::License_rst_len}},
        {"fmtlib", {reinterpret_cast<char*>(fmtlib::LICENSE_rst), fmtlib::LICENSE_rst_len}},
    }};
    return ret;
}

} // namespace scraps
