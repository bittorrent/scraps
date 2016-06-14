#pragma once

#include "scraps/config.h"

#include "scraps/logging.h"
#include "scraps/thread.h"
#include "scraps/net/utility.h"

#include <unordered_map>

#include <unistd.h>

SCRAPS_IGNORE_WARNINGS_PUSH
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/placeholders.hpp>

#include <boost/bind.hpp>
SCRAPS_IGNORE_WARNINGS_POP

#include <condition_variable>

namespace scraps {
namespace net {

/**
* Implements a tcp server. ConnectionClass should be a class with a run() method, to
* be invoked from a new thread, and a cancel() method to cause the run() invocation to return
* early. The Args parameter represents the types of the arguments to be passed to new
* ConnectionClass instances.
*/
template <typename ConnectionClass, typename... Args>
class TCPAcceptor {
public:
    /**
    * Constructs a new server. Any arguments passed to the constructor will be copied and
    * passed to each connection instance, following the native socket handle.
    */
    TCPAcceptor(const Args&... args) : _args(args...), _acceptor(_service), _socket(_service) {}
    TCPAcceptor(const TCPAcceptor& other) = delete;
    TCPAcceptor& operator=(const TCPAcceptor& other) = delete;
    ~TCPAcceptor() { _stop(); }

    /**
    * Starts the server on the specified address and port.
    *
    * @param address the address to start the server on
    * @param port the port to start the server on
    */
    bool start(Address address, uint16_t port) {
        std::unique_lock<std::mutex> lock(_startStopMutex);
        _stop();
        _service.reset();
        _isCancelled = false;

        try {
            _acceptor.open(address.is_v6() ? boost::asio::ip::tcp::v6() : boost::asio::ip::tcp::v4());
            _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            _acceptor.bind(boost::asio::ip::tcp::endpoint(address, port));
            _acceptor.listen();
        } catch (std::exception& e) {
            SCRAPS_LOGF_ERROR("exception opening tcp server acceptor: %s", e.what());
            return false;
        }

        _work = std::make_unique<boost::asio::io_service::work>(_service);

        _serviceThread = std::thread([&] {
            SCRAPS_LOGF_INFO("starting tcp server worker thread");
            while (!_isCancelled) {
                try {
                    // TODO: this should just be `_service.run()`, but that currently breaks android compilation
                    const_cast<boost::asio::io_service&>(_service).run();
                    break;
                } catch (std::exception& e) {
                    SCRAPS_LOGF_ERROR("exception in tcp server worker thread: %s", e.what());
                } catch (...) {
                    SCRAPS_LOGF_ERROR("unknown exception in tcp server worker thread");
                }
            }
            SCRAPS_LOGF_INFO("exiting tcp server worker thread");
        });

        _connectionCleaner = std::thread([&] {
            while (!_isCancelled) {
                std::unique_lock<std::mutex> lock{_mutex};
                _cv.wait(lock, [&]{return !_completeConnections.empty() || _isCancelled;});

                for (auto connection : _completeConnections) {
                    _connections.erase(connection);
                }

                auto completeConnections = std::move(_completeConnections);
                _completeConnections.clear();
                lock.unlock();
            }
        });

        _acceptor.async_accept(
            _socket, boost::bind(ArgsHelper<sizeof...(Args)>::AcceptHandler(), this, boost::asio::placeholders::error));

        return true;
    }

    /**
    * Stops the server.
    */
    void stop() {
        std::lock_guard<std::mutex> lock(_startStopMutex);
        _stop();
    }

    template <typename Callback>
    void iterateConnections(Callback&& callback) {
        std::vector<std::weak_ptr<ConnectionClass>> connections;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            for (auto& kv : _connections) {
                connections.emplace_back(kv.second->connectionClass);
            }
        }
        for (auto& connection : connections) {
            if (auto shared = connection.lock()) {
                callback(*shared);
            }
        }
    }

private:
    std::atomic<bool> _isCancelled{false};

    std::mutex _mutex;
    std::mutex _startStopMutex;

    std::tuple<Args...> _args;

    boost::asio::io_service _service;
    std::unique_ptr<boost::asio::io_service::work> _work;
    std::thread _serviceThread;

    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::ip::tcp::socket _socket;

    struct Connection {
        ~Connection() {
            auto connection = connectionClass.lock();
            if (connection) {
                connection->cancel();
            }

            if (connectionThread.joinable()) {
                connectionThread.join();
            }
        }
        std::weak_ptr<ConnectionClass> connectionClass;
        std::thread connectionThread;
    };
    std::unordered_map<ConnectionClass*, std::shared_ptr<Connection>> _connections;
    std::vector<ConnectionClass*> _completeConnections;
    std::condition_variable _cv;
    std::thread _connectionCleaner;

    template <int N, int... S>
    struct ArgsHelper : ArgsHelper<N - 1, N - 1, S...> {};

    template <int... S>
    struct ArgsHelper<0, S...> {
        typedef void (TCPAcceptor::*AcceptHandlerPointer)(const boost::system::error_code&);
        static constexpr AcceptHandlerPointer AcceptHandler() { return &TCPAcceptor::_acceptHandler<S...>; }
    };

    void _stop() {
        _isCancelled = true;
        _cv.notify_one();

        try {
            _acceptor.close();
        } catch (...) {}

        _work.reset();

        if (_serviceThread.joinable()) {
            _serviceThread.join();
        }

        if (_connectionCleaner.joinable()) {
            _cv.notify_one();
            _connectionCleaner.join();
        }

        _connections.clear();
        _completeConnections.clear();
    }

    template <int... ArgIndices>
    void _acceptHandler(const boost::system::error_code& error) {
        if (error) {
            if (error != boost::asio::error::operation_aborted) {
                SCRAPS_LOGF_ERROR("tcp server error: %s", error.message().c_str());
            }
            return;
        }

#if _WIN32
        int fd = INVALID_SOCKET;
        WSAPROTOCOL_INFO protocolInfo;
        if (!WSADuplicateSocket(_socket.native_handle(), GetCurrentProcessId(), &protocolInfo)) {
            fd = WSASocket(protocolInfo.iAddressFamily,
                           protocolInfo.iSocketType,
                           protocolInfo.iProtocol,
                           &protocolInfo,
                           0,
                           WSA_FLAG_OVERLAPPED);
        }
        if (fd == INVALID_SOCKET) {
#else
        int fd = dup(_socket.native_handle());
        if (fd == -1) {
#endif
            SCRAPS_LOGF_ERROR("error duplicating socket handle (errno = %d)", SocketError());
            return;
        }

        try {
            _socket.close();
        } catch (...) {}

        auto connection = std::make_shared<Connection>();
        auto connectionClass = std::make_shared<ConnectionClass>(fd, std::get<ArgIndices>(_args)...);
        connection->connectionClass = connectionClass;
        connection->connectionThread = std::thread([&, connectionClass]() {
            connectionClass->run();
            {
                std::lock_guard<std::mutex> lock{_mutex};
                _completeConnections.emplace_back(connectionClass.get());
            }
            _cv.notify_one();
        });

        std::lock_guard<std::mutex> lock{_mutex};
        _connections.emplace(connectionClass.get(), connection);
        _acceptor.async_accept(
            _socket, boost::bind(&TCPAcceptor::_acceptHandler<ArgIndices...>, this, boost::asio::placeholders::error));
    }
};

}} // namespace scraps::net
