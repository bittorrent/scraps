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
