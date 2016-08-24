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
