#include "scraps/RunLoop.h"

#include <gtest/gtest.h>

#include <thread>
#include <sys/socket.h>

using namespace scraps;

TEST(RunLoop, async) {
    RunLoop runLoop;

    int x = 0;
    std::chrono::steady_clock::time_point asyncTime;

    runLoop.async([&] {
        EXPECT_EQ(x, 1);
        asyncTime = std::chrono::steady_clock::now();
        runLoop.async([&] {
            auto now = std::chrono::steady_clock::now();
            EXPECT_EQ(x, 2);
            EXPECT_GE(now - asyncTime, 200ms);
            x = 3;
            runLoop.cancel();
        }, 200ms);
        x = 2;
    });

    x = 1;
    runLoop.run();

    EXPECT_EQ(x, 3);
};

TEST(RunLoop, events) {
    RunLoop runLoop;

    int sockets[2];
    EXPECT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets), 0);

    runLoop.add(sockets[0], POLLIN);
    runLoop.add(sockets[1], POLLIN);

    runLoop.setEventHandler([&](int fd, short events) {
        EXPECT_EQ(fd, sockets[1]);
        EXPECT_EQ(events, POLLIN);
        char buf[10];
        EXPECT_EQ(recv(sockets[1], buf, sizeof(buf), 0), 2);
        EXPECT_EQ(memcmp(buf, "hi", 2), 0);
        runLoop.cancel();
    });

    send(sockets[0], "hi", 2, 0);

    runLoop.run();

    close(sockets[0]);
    close(sockets[1]);
};

TEST(RunLoop, cancelDoesntDeadlock) {
    // Ensures that canceling the run immediately after starting it doesn't
    // produce a deadlock.
    RunLoop runLoop;

    auto thread = std::thread([&]{ runLoop.run(); });

    runLoop.cancel();

    if (thread.joinable()) {
        thread.join();
    }
}
