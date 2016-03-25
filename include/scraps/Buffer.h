#pragma once

#include "scraps/config.h"

namespace scraps {

template <typename T>
class Buffer {
public:
    Buffer() {}

    /**
    * Wraps the given data. The buffer does not copy it or gain ownership.
    */
    Buffer(T* data, size_t size) : _data(data), _size(size) {}

    Buffer& operator=(const Buffer& other) {
        _data       = other._data;
        _size       = other._size;
        return *this;
    }

    T* data() const { return _data; }
    size_t size() const { return _size; }

private:
    T* _data     = nullptr;
    size_t _size = 0;
};

} // namespace scraps
