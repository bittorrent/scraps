#pragma once

#include "scraps/config.h"

#include <string>

namespace scraps {

class SlidingBuffer {
public:
    SlidingBuffer() = default;
    SlidingBuffer(const void* data, size_t size) : _buffer(static_cast<const char*>(data), size) {}

    const void* data() const { return _buffer.data() + _offset; }
    size_t size() const { return _buffer.size() - _offset; }

    bool empty() const { return !size(); }

    void clear() {
        _buffer.clear();
        _offset = 0;
    }

    void assign(const void* data, size_t size) {
        _buffer.assign(static_cast<const char*>(data), size);
        _offset = 0;
    }

    void push(const void* data, size_t size) { _buffer.append(static_cast<const char*>(data), size); }

    void pop(size_t size) {
        _offset += size;
        if (_offset >= _buffer.size()) {
            _buffer.clear();
            _offset = 0;
        } else if (_offset > ((_buffer.size() - _offset) << 2)) {
            _buffer = _buffer.substr(_offset);
            _offset = 0;
        }
    }

private:
    std::string _buffer;
    size_t _offset = 0;
};

} // namespace scraps
