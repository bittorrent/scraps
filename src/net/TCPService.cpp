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
#include <scraps/net/TCPService.h>

#include <scraps/logging.h>
#include <scraps/utility.h>
#include <scraps/net/Endpoint.h>
#include <scraps/net/utility.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <strings.h>

#include <thread>

namespace scraps::net {

TCPService::Connection::~Connection() {
    close();
    if (fd >= 0) {
        ::close(fd);
    }
}

bool TCPService::Connection::close() {
    if (isClosing()) {
        return true;
    } else if (isConnecting()) {
        state = kClosing;
    } else if (fd < 0) {
        SCRAPS_LOG_INFO("fd < 0 (fd = {})", fd);
        return false;
    } else {
        state = kClosing;
        SCRAPS_LOG_INFO("shutting down connection (fd = {})", fd);
        ::shutdown(fd, SHUT_WR);
    }

    return true;
}

TCPService::~TCPService() {
    stop();
    wait();
}

bool TCPService::bind(const std::string& interface, uint16_t port) {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    if (::inet_pton(addr.sin_family, interface.c_str(), &addr.sin_addr.s_addr) <= 0) {
        SCRAPS_LOG_ERROR("invalid interface");
        return false;
    }
    addr.sin_port = htons(port);

    auto fd = ::socket(addr.sin_family, SOCK_STREAM, 0);
    if (fd < 0) {
        SCRAPS_LOG_ERROR("error opening socket (errno = {})", static_cast<int>(errno));
        return false;
    }

    int reuse = 1;
    ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    if (::bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        SCRAPS_LOG_ERROR("error binding socket (errno = {})", static_cast<int>(errno));
        ::close(fd);
        return false;
    }

    if (!SetBlocking(fd, false)) {
        SCRAPS_LOG_ERROR("couldn't make socket non-blocking");
        ::close(fd);
        return false;
    }

    ::listen(fd, 20);
    _listenFD = fd;

    return true;
}

uint16_t TCPService::port() const {
    struct sockaddr_in addr;
    socklen_t saLen = sizeof(addr);
    int rc = ::getsockname(_listenFD, reinterpret_cast<struct sockaddr*>(&addr), &saLen);
    if (rc == 0) {
        return ntohs(addr.sin_port);
    } else {
        return 0;
    }
}

void TCPService::start() {
    stop();
    wait();

    _runLoop.reset();
    _runLoop.setEventHandler([&](int fd, short events) {
        _eventHandler(fd, events);
    });

    if (_listenFD != -1) {
        _runLoop.async([this] {
            _runLoop.add(_listenFD, POLLIN);
        });
    }

    _thread = std::thread([&] {
        SetThreadName(typeid(*_delegate).name());

        _runLoop.run();

        while (!_connections.empty()) {
            _closeAndErase(*_connections.front());
        }

        if (_listenFD >= 0) {
            _runLoop.remove(_listenFD);
            ::close(_listenFD);
            _listenFD = -1;
        }

        _runLoop.flush();
    });
}

void TCPService::stop() {
    _runLoop.cancel();
}

void TCPService::wait() const {
    if (_thread.joinable()) {
        _thread.join();
    }
}

void TCPService::async(const std::function<void()>& function, std::chrono::milliseconds delay) {
    _runLoop.async(function, delay);
}

bool TCPService::isCurrentThread() const {
    return std::this_thread::get_id() == _thread.get_id();
}

TCPService::ConnectionId TCPService::connect(sockaddr* address, size_t addressLength) {
    ConnectionId id = ++_connectionIdCounter;

    sockaddr_storage addressStorage;
    memcpy(&addressStorage, address, addressLength);

    _runLoop.async([this, id, addressStorage, addressLength] {
        _connect(id, reinterpret_cast<const sockaddr*>(&addressStorage), addressLength);
    });

    return id;
}

TCPService::ConnectionId TCPService::connect(const std::string& host, uint16_t port) {
    ConnectionId id = ++_connectionIdCounter;

    _runLoop.async([this, host, port, id] {
        auto addresses = ResolveIPv4(host);
        if (addresses.empty()) {
            SCRAPS_LOG_ERROR("unable to resolve host {}", host);
            return;
        }
        Endpoint endpoint{addresses[UniformDistribution(_prng, 0, addresses.size() - 1)], port};
        sockaddr_storage addr;
        socklen_t addrLength;
        endpoint.getSockAddr(&addr, &addrLength);
        _connect(id, reinterpret_cast<sockaddr*>(&addr), addrLength);
    });

    return id;
}

void TCPService::send(TCPService::ConnectionId connectionId, const void* data, size_t length) {
    auto buffer = std::make_shared<Connection::SendBuffer>();
    buffer->data.assign(static_cast<const char*>(data), length);

    _runLoop.async([this, connectionId, buffer] {
        auto connection = _connections.findById(connectionId);
        if (!connection) { return; }

        if (connection->isClosing()) { return; }

        connection->sendQueue.push(buffer);

        _trySend(*connection);
    });
}

void TCPService::close(TCPService::ConnectionId connectionId) {
    SCRAPS_LOG_INFO("closing tcp connection (id = {})", connectionId);

    _runLoop.async([this, connectionId] {
        auto connection = _connections.findById(connectionId);
        if (!connection) { return; }

        if (!connection->close()) {
            _closeAndErase(*connection);
        }
    });
}

void TCPService::_eventHandler(int fd, short events) {
    if (fd == _listenFD) {
        // TODO: limit or filter our connections instead of blindly accepting everything?
        auto newFD = accept(fd, nullptr, nullptr);
        if (newFD < 0) {
            SCRAPS_LOG_ERROR("error accepting connection");
            return;
        }
        if (!SetBlocking(newFD, false)) {
            SCRAPS_LOG_ERROR("couldn't make socket non-blocking");
            ::close(newFD);
            return;
        }
        SCRAPS_LOG_INFO("accepted connection (fd = {})", newFD);
        _runLoop.add(newFD, POLLIN | POLLOUT | POLLHUP);
        auto id = ++_connectionIdCounter;
        _connections.emplace(id, newFD, Connection::kConnected);
        _delegate->tcpServiceConnectionEstablished(id);
        return;
    }

    auto connection = _connections.findByFd(fd);
    if (!connection) {
        // file descriptor is already closed...remove it from the run loop
        _runLoop.remove(fd);
        return;
    }

    if ((events & POLLHUP) || (connection->isClosing() && (events & POLLIN))) {
        // finishing up a graceful shutdown
        SCRAPS_LOG_INFO("connection closed (fd = {})", fd);

        shutdown(fd, SHUT_WR);
        recv(fd, nullptr, 0, 0);

        _closeAndErase(*connection);
        return;
    }

    if (events & POLLOUT) {
        if (connection->isConnecting()) {
            int err = 0;
            socklen_t errSize = sizeof(err);
            if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &errSize) < 0 || err) {
                SCRAPS_LOG_INFO("connection failed (fd = {}, err = {})", fd, err);
                _delegate->tcpServiceConnectionFailed(connection->id);
            } else {
                SCRAPS_LOG_INFO("connection established (fd = {})", fd);
                connection->state = Connection::kConnected;
                _delegate->tcpServiceConnectionEstablished(connection->id);
            }
        }

        _trySend(*connection);
    }

    if (events & POLLIN) {
        while (true) {
            char buf[500];
            auto bytes = recv(fd, buf, sizeof(buf), 0);

            if (bytes < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
                break;
            }

            if (bytes <= 0) {
                if (bytes < 0) {
                    SCRAPS_LOG_ERROR("socket error (fd = {}, errno = {})", fd, static_cast<int>(errno));
                } else {
                    SCRAPS_LOG_INFO("closing connection (fd = {})", fd);
                }

                _closeAndErase(*connection);
                return;
            }

            _delegate->tcpServiceConnectionReceivedData(connection->id, buf, static_cast<size_t>(bytes));
        }
    }
}

void TCPService::_connect(TCPService::ConnectionId connectionId, const sockaddr* address, size_t addressLength) {
    auto s = socket(address->sa_family, SOCK_STREAM, 0);

    if (s < 0) {
        SCRAPS_LOG_ERROR("error opening socket (errno = {})", static_cast<int>(errno));
    } else if (!SetBlocking(s, false)) {
        SCRAPS_LOG_ERROR("couldn't make socket non-blocking");
        ::close(s);
        s = -1;
    } else if (::connect(s, address, addressLength) < 0 && errno != EINPROGRESS) {
        SCRAPS_LOG_ERROR("error connecting socket (errno = {})", static_cast<int>(errno));
        ::close(s);
        s = -1;
    }

    int one = 1;
    if (s >= 0 && setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one)) < 0) {
        SCRAPS_LOG_WARNING("unable to set TCP_NODELAY on socket (errno = {})", static_cast<int>(errno));
    }

    if (s < 0) {
        _delegate->tcpServiceConnectionFailed(connectionId);
        return;
    }

    _connections.emplace(connectionId, s, Connection::kConnecting);

    _runLoop.add(s, POLLIN | POLLOUT | POLLHUP);
}

void TCPService::_trySend(Connection& connection) {
    if (!connection.isConnected()) { return; }

    while (!connection.sendQueue.empty()) {
        auto& buffer = connection.sendQueue.front();

        auto remaining = buffer->data.size() - buffer->sent;

        if (!remaining) {
            connection.sendQueue.pop();
            continue;
        }

        auto sent = ::send(connection.fd, static_cast<const char*>(buffer->data.data()) + buffer->sent, remaining, 0);

        if (sent < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            } else {
                SCRAPS_LOG_ERROR("socket send error (errno = {})", static_cast<int>(errno));
                _closeAndErase(connection);
                return;
            }
        }

        if (static_cast<size_t>(sent) < remaining) {
            buffer->sent += sent;
            break;
        }

        connection.sendQueue.pop();
    }

    _runLoop.add(connection.fd, POLLIN | (connection.sendQueue.empty() ? 0 : POLLOUT) | POLLHUP);
}

void TCPService::_closeAndErase(Connection& connection) {
    auto id = connection.id;
    auto wasConnecting = connection.isConnecting();

    _runLoop.remove(connection.fd);
    _connections.erase(connection.id);

    if (wasConnecting) {
        _delegate->tcpServiceConnectionFailed(id);
    } else {
        _delegate->tcpServiceConnectionClosed(id);
    }
}

} // namespace scraps::net
