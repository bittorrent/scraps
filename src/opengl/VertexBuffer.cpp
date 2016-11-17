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
#include <scraps/opengl/VertexBuffer.h>

namespace scraps::opengl {

VertexBuffer::VertexBuffer() {
    glGenBuffers(1, &_object);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &_object);
}

void VertexBuffer::bind(GLenum target) {
    glBindBuffer(target, _object);
    _currentTarget = target;
}

void VertexBuffer::unbind() {
    if (_currentTarget) {
        glBindBuffer(_currentTarget, 0);
        _currentTarget = 0;
    }
}

void VertexBuffer::store(const uint8_t* data, size_t size, GLenum usage) {
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    unbind();
}

} // namespace scraps::opengl
