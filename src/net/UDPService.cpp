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
#include <scraps/net/UDPService.h>

#include <scraps/logging.h>

namespace scraps {
namespace net {

UDPService::UDPService() {
    _runLoop.setEventHandler([this](int fd, short events) {
        std::lock_guard<std::mutex> lock(_mutex);

        auto it = _sockets.find(fd);
        if (it == _sockets.end()) { return; }

        auto socket = it->second.lock();
        if (!socket) {
            _sockets.erase(it);
            _runLoop.remove(fd);
            return;
        }

        if (events & POLLIN) {
            socket->receive();
        }
    });

    SCRAPS_LOGF_INFO("starting udp service thread");
    _thread = std::thread([this] {
        _runLoop.run();
    });
}

UDPService::~UDPService() {
    kill();
}

void UDPService::kill() {
    if (_thread.joinable()) {
        SCRAPS_LOGF_INFO("stopping udp service thread");
        _runLoop.cancel();
        _runLoop.flush();
        _thread.join();
    }
}

std::shared_ptr<UDPSocket> UDPService::openSocket(UDPSocket::Protocol protocol, uint16_t port, std::weak_ptr<UDPReceiver> receiver, const char* interface) {
    std::lock_guard<std::mutex> lock(_mutex);
    _purgeDeadSockets();

    auto socket = std::make_shared<UDPSocket>(protocol, receiver);
    if (interface) {
        if (!socket->bind(interface, port)) {
            return nullptr;
        }
    } else if (!socket->bind(port)) {
        return nullptr;
    }

    auto fd = socket->native();
    _sockets[fd] = socket;
    _runLoop.add(fd, POLLIN);
    return socket;
}

std::shared_ptr<UDPSocket> UDPService::openMulticastSocket(const Address& groupAddress, uint16_t port, std::weak_ptr<UDPReceiver> receiver) {
    std::lock_guard<std::mutex> lock(_mutex);
    _purgeDeadSockets();

    auto protocol = groupAddress.is_v4() ? UDPSocket::Protocol::kIPv4 : UDPSocket::Protocol::kIPv6;
    auto socket = std::make_shared<UDPSocket>(protocol, receiver);
    if (!socket->bindMulticast(groupAddress.to_string().c_str(), port)) {
        return nullptr;
    }

    auto fd = socket->native();
    _sockets[fd] = socket;
    _runLoop.add(fd, POLLIN);
    return socket;
}

void UDPService::_purgeDeadSockets() {
    for (auto it = _sockets.begin(); it != _sockets.end();) {
        if (!it->second.lock()) {
            _runLoop.remove(it->first);
            it = _sockets.erase(it);
        } else {
            ++it;
        }
    }
}

}} // namespace scraps::net
