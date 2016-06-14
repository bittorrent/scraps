#include "scraps/net/URL.h"

#include <cstdlib>

namespace scraps {
namespace net {

namespace {

std::tuple<std::string, std::string, uint16_t, std::string> Parse(const std::string& url) {
    const auto protocolEnd = url.find("://");
    std::string protocol   = protocolEnd == url.npos ? "http" : url.substr(0, protocolEnd);

    const auto hostStart = protocolEnd == url.npos ? 0 : protocolEnd + 3;

    auto hostEnd = url.find('/', hostStart);

    uint16_t port = 0;
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
    auto host       = url.substr(hostStart, hostLength);

    const auto portEnd       = url.find('/', hostStart + hostLength);
    const auto resourceStart = portEnd == url.npos ? url.size() : portEnd;
    auto resource            = url.substr(resourceStart);

    return std::make_tuple(std::move(protocol), std::move(host), port, std::move(resource));
}

} // {anonymous} namespace

std::string URL::protocol() const { return std::get<0>(Parse(_url)); }

std::string URL::host() const { return std::get<1>(Parse(_url)); }

uint16_t URL::port() const { return std::get<2>(Parse(_url)); }

std::string URL::resource() const { return std::get<3>(Parse(_url)); }

}} // namespace scraps::net
