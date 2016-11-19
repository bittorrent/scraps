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
#pragma once

#include <scraps/config.h>

#include <vector>

namespace scraps {

template <typename T>
class CircularBuffer {
public:
    CircularBuffer(size_t capacity = 0) { alloc(capacity); }

    void alloc(size_t count) {
        _buffer.resize(count);
        _front = _back = 0;
        _isEmpty = true;
    }

    bool empty() const { return _isEmpty; }

    void push(const T* elements, size_t count) {
        while (count) {
            if (count <= _buffer.size() - _back) {
                memcpy(_buffer.data() + _back, elements, count * sizeof(T));
                if (_isEmpty) {
                    // before: -----X--------------
                    // after:  -----F---------B----
                    _back += count;
                } else if (_front >= _back && count >= (_front - _back)) {
                    // before: -----B----F---------
                    // after:  ---------------X----
                    _front = _back = _back + count;
                } else {
                    // before: -----B----F---------
                    // after:  -------B--F---------
                    // or
                    // before: -----F----B---------
                    // after:  -----F----------B---
                    _back += count;
                }
                count = 0;
            } else {
                memcpy(_buffer.data() + _back, elements, (_buffer.size() - _back) * sizeof(T));
                count -= (_buffer.size() - _back);
                elements += (_buffer.size() - _back);
                if (_front < _back) {
                    // before: ----F--------B-------
                    // after:  B---F----------------
                    _back = 0;
                } else {
                    // before: ------B----F---------
                    // after:  X--------------------
                    _front = _back = 0;
                }
            }
            _isEmpty = false;
            if (_back == _buffer.size()) {
                _back = 0;
            }
            if (_front == _buffer.size()) {
                _front = 0;
            }
        }
    }

    void push(const T& element) { push(&element, 1); }

    void pop(size_t count) {
        while (count && !_isEmpty) {
            if (_front < _back) {
                if (count >= _back - _front) {
                    _front = _back = 0;
                    _isEmpty = true;
                    return;
                }
                _front += count;
                return;
            }

            if (count >= _buffer.size() - _front) {
                count -= _buffer.size() - _front;
                _front = 0;
            } else {
                _front += count;
                return;
            }
        }
    }

    T pop_front() {
        auto ret = _buffer[_front];
        ++_front;
        if (_front == _buffer.size()) {
            _front = 0;
        }
        if (_front == _back) {
            _isEmpty = true;
        }
        return ret;
    }

    size_t size() const {
        if (_isEmpty) {
            return 0;
        }
        return (_back > _front) ? (_back - _front) : (_buffer.size() - _front + _back);
    }

    size_t contiguousSize() const {
        if (_isEmpty) {
            return 0;
        }
        return (_back > _front) ? (_back - _front) : (_buffer.size() - _front);
    }

    const T* contiguousData() const { return _buffer.data() + _front; }

    void clear() {
        _front = _back = 0;
        _isEmpty = true;
    }

    struct ConstIterator : std::iterator<std::forward_iterator_tag, T> {
        ConstIterator(const CircularBuffer* buffer, size_t position) : _buffer(buffer), _position(position) {}
        ConstIterator(const ConstIterator& other) : _buffer(other._buffer), _position(other._position) {}

        bool operator==(const ConstIterator& other) const {
            return other._buffer == _buffer && other._position == _position;
        }
        bool operator!=(const ConstIterator& other) const { return !(*this == other); }
        const T& operator*() const { return _buffer->_buffer[_position]; }
        const T& operator->() const { return _buffer->_buffer[_position]; }

        ConstIterator& operator++() {
            ++_position;
            if (_position == _buffer->_buffer.size()) {
                _position = 0;
            }
            if (_position == _buffer->_back) {
                _position = _buffer->_buffer.capacity();
            }
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator ret{*this};
            ++*this;
            return ret;
        }

        const CircularBuffer* _buffer;
        size_t _position = 0;
    };

    ConstIterator begin() const { return ConstIterator(this, empty() ? _buffer.capacity() : _front); }
    ConstIterator end() const { return ConstIterator(this, _buffer.capacity()); }

private:
    std::vector<T> _buffer;
    size_t _front = 0;
    size_t _back  = 0;
    bool _isEmpty = true;
};

} // namespace scraps
