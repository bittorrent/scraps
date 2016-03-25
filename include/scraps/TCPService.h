#pragma once

#include "scraps/config.h"

#include "scraps/RunLoop.h"
#include "scraps/thread.h"
#include <unordered_map>

#include <netinet/in.h>

#include <atomic>
#include <memory>
#include <queue>

namespace scraps {

struct TCPServiceDelegate;

/**
* Thread-safe.
*/
class TCPService {
public:
    TCPService(TCPServiceDelegate* delegate) : _delegate(delegate), _connectionIdCounter(0) {}
    ~TCPService();

    /**
    * Binds the service to interface and port. If the port is not given, an
    * open port will be chosen by the operating system. This can be retrieved
    * with the port() method.
    *
    * start() should be called afterwards to start listening.
    *
    * Returns true if successful.
    */
    bool bind(const std::string& interface, uint16_t port = 0);

    /**
    * Returns the currently bound listening port.
    */
    uint16_t port() const;

    /**
    * Starts the service.
    */
    void start();

    /**
    * Stops the service. Use wait() to wait for it to fully stop.
    */
    void stop();

    /**
    * Blocks until the service is fully stopped.
    */
    void wait() const;

    /**
    * Sets a function to be invoked by the service thread after a specified delay.
    */
    void async(const std::function<void()>& function, std::chrono::milliseconds delay = std::chrono::milliseconds(0));

    /**
    * Indicates whether or not the current thread belongs to the service.
    */
    bool isCurrentThread() const;

    typedef uint32_t ConnectionId;

    /**
    * Initiates a connection to the address.
    *
    * @return the id of the new connection
    */
    ConnectionId connect(sockaddr* address, size_t addressLength);

    /**
    * Initiates a connection to the address.
    *
    * @return the id of the new connection
    */
    ConnectionId connect(const std::string& host, uint16_t port);

    /**
    * Sends data through the given connection.
    *
    * If the connection isn't yet established or the data can't be sent immediately, it's buffered and sent as soon as
    * possible.
    */
    void send(ConnectionId connectionId, const void* data, size_t length);

    /**
    * Sends data through the given connection.
    *
    * If the connection isn't yet established or the data can't be sent immediately, it's buffered and sent as soon as
    * possible.
    */
    void send(ConnectionId connectionId, const std::string& data) { send(connectionId, data.data(), data.size()); }

    /**
    * Gracefully closes the given connection.
    *
    * Any previously buffered data is sent before the connection is closed.
    */
    void close(ConnectionId connectionId);

private:
    TCPServiceDelegate* const _delegate = nullptr;

    mutable std::thread _thread;

    RunLoop _runLoop;

    std::atomic<ConnectionId> _connectionIdCounter;

    int _listenFD = -1;

    struct Connection {
        const ConnectionId id;
        const int fd;
        enum State { kConnecting, kConnected, kClosing, kClosed };

        State state;

        struct SendBuffer {
            std::string data;
            size_t sent = 0;
        };

        std::queue<std::shared_ptr<SendBuffer>> sendQueue;

        Connection(ConnectionId connectionId, int fd, State state) : id(connectionId), fd(fd), state(state) {
            SCRAPS_ASSERT(fd >= 0);
        }
        ~Connection();

        bool isConnecting() const { return state == kConnecting; }
        bool isConnected() const { return state == kConnected; }
        bool isClosing() const { return state == kClosing; }
        bool isClosed() const { return state == kClosed; }

        bool close();
    };

    struct ConnectionMap {
        bool empty() const { return _idMap.empty(); }

        std::shared_ptr<Connection> front() const { return _idMap.begin()->second; }

        template <typename... Args>
        std::shared_ptr<Connection> emplace(Args... args) {
            auto connection = std::make_shared<Connection>(std::forward<Args>(args)...);
            _idMap.emplace(connection->id, connection);
            _fdMap.emplace(connection->fd, connection);
            SCRAPS_ASSERT(_fdMap.size() == _idMap.size());
            return connection;
        }

        void erase(ConnectionId connectionId) {
            auto it = _idMap.find(connectionId);
            if (it == _idMap.end()) {
                return;
            }
            auto& connection = *it->second;
            connection.close();
            _fdMap.erase(connection.fd);
            _idMap.erase(it);
            SCRAPS_ASSERT(_fdMap.size() == _idMap.size());
        }

        std::shared_ptr<Connection> findById(ConnectionId connectionId) const {
            auto it = _idMap.find(connectionId);
            if (it == _idMap.end()) {
                return nullptr;
            }
            return it->second;
        }

        std::shared_ptr<Connection> findByFd(int fd) const {
            auto it = _fdMap.find(fd);
            if (it == _fdMap.end()) {
                return nullptr;
            }
            return it->second;
        }

    private:
        std::unordered_map<ConnectionId, std::shared_ptr<Connection>> _idMap;
        std::unordered_map<int, std::shared_ptr<Connection>> _fdMap;
    };

    ConnectionMap _connections;

    void _eventHandler(int fd, short events);

    void _connect(ConnectionId connectionId, const sockaddr* address, size_t addressLength);
    void _trySend(Connection& connection);
    void _closeAndErase(Connection& connection);
};

struct TCPServiceDelegate {
    virtual void tcpServiceConnectionEstablished(TCPService::ConnectionId connectionId) {}
    virtual void tcpServiceConnectionFailed(TCPService::ConnectionId connectionId) {}
    virtual void tcpServiceConnectionReceivedData(TCPService::ConnectionId connectionId,
                                                  const void* data,
                                                  size_t length) {}
    virtual void tcpServiceConnectionClosed(TCPService::ConnectionId connectionId) {}
};

} // namespace scraps
