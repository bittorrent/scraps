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
#include "scraps/opengl/opengl.h"

namespace scraps {
namespace opengl {

class VertexBuffer {
public:
    VertexBuffer();
    ~VertexBuffer();

    GLuint id() const { return _object; }

    void bind(GLenum target = GL_ARRAY_BUFFER);
    void unbind();

    void store(const uint8_t* data, size_t size, GLenum usage = GL_STATIC_DRAW);

    template <typename T>
    void store(const T* data, size_t count, GLenum usage = GL_STATIC_DRAW) {
        store(reinterpret_cast<const uint8_t*>(data), count * sizeof(T), usage);
    }

    template <typename T>
    void stream(const T* data, size_t count) {
        store(data, count, GL_STREAM_DRAW);
    }

private:
    GLuint _object = 0;
    GLenum _currentTarget = 0;
};

} // namespace opengl
} // namespace scraps
