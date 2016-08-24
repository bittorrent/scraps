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
#include "scraps/opengl/OpenGL.h"
#include "scraps/opengl/VertexArray.h"
#include "scraps/opengl/VertexBuffer.h"

namespace scraps {
namespace opengl {

/**
* Convenience class for when you just need a vertex array with a single buffer.
*/
class VertexArrayBuffer : public VertexArray, public VertexBuffer {
public:
    void bind() { VertexArray::bind(); VertexBuffer::bind(); }
    void unbind() { VertexArray::unbind(); VertexBuffer::unbind(); }

    void setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset) {
        VertexArray::setAttribute(index, *this, size, type, normalized, stride, offset);
    }
};

} // namespace opengl
} // namespace scraps
