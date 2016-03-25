#pragma once

#include "scraps/config.h"

#include "scraps/RunLoop.h"

#include "hiredis/async.h"

namespace scraps {

class RedisRunLoopAttachment {
public:
    static std::unique_ptr<RedisRunLoopAttachment> Create(redisAsyncContext* redisAsyncCtx, RunLoop* runLoop) {
        return std::unique_ptr<RedisRunLoopAttachment>(new RedisRunLoopAttachment(redisAsyncCtx, runLoop));
    }

    void handleEvents(int fd, short events) {
        if (fd != _redis->c.fd) { return; }

        if (events & POLLIN) {
            redisAsyncHandleRead(_redis);
        } else if (events & POLLOUT) {
            redisAsyncHandleWrite(_redis);
        }
    }

private:
    redisAsyncContext* _redis = nullptr;
    RunLoop* _runLoop = nullptr;
    short _events = 0;

    RedisRunLoopAttachment(redisAsyncContext* redisAsyncCtx, RunLoop* runLoop)
        : _redis{redisAsyncCtx}, _runLoop{runLoop}
    {
        redisAsyncCtx->ev.addRead  = &AddRead;
        redisAsyncCtx->ev.delRead  = &DelRead;
        redisAsyncCtx->ev.addWrite = &AddWrite;
        redisAsyncCtx->ev.delWrite = &DelWrite;
        redisAsyncCtx->ev.cleanup  = &Cleanup;
        redisAsyncCtx->ev.data     = this;
        _runLoop->add(redisAsyncCtx->c.fd, POLLIN | POLLOUT);
    }

    static void AddRead(void* privdata) {
        auto self = reinterpret_cast<RedisRunLoopAttachment*>(privdata);
        self->_events |= POLLIN;
        self->_runLoop->add(self->_redis->c.fd, self->_events);
    }

    static void DelRead(void* privdata) {
        auto self = reinterpret_cast<RedisRunLoopAttachment*>(privdata);
        self->_events &= ~POLLIN;
        if (!self->_events) {
            self->_runLoop->remove(self->_redis->c.fd);
        }
    }

    static void AddWrite(void* privdata) {
        auto self = reinterpret_cast<RedisRunLoopAttachment*>(privdata);
        self->_events |= POLLOUT;
        self->_runLoop->add(self->_redis->c.fd, self->_events);
    }

    static void DelWrite(void* privdata) {
        auto self = reinterpret_cast<RedisRunLoopAttachment*>(privdata);
        self->_events &= ~POLLOUT;
        if (!self->_events) {
            self->_runLoop->remove(self->_redis->c.fd);
        }
    }

    static void Cleanup(void* privdata) {
        auto self = reinterpret_cast<RedisRunLoopAttachment*>(privdata);
        self->_runLoop->remove(self->_redis->c.fd);
    }
};

} // namespace scraps
