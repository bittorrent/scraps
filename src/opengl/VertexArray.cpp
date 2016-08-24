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
#include "scraps/opengl/VertexArray.h"

namespace scraps {
namespace opengl {

#if SCRAPS_IOS || SCRAPS_TVOS
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#endif

VertexArray::VertexArray() {
#if !SCRAPS_EMULATE_VERTEX_ARRAY
    glGenVertexArrays(1, &_object);
#endif
}

VertexArray::~VertexArray() {
#if !SCRAPS_EMULATE_VERTEX_ARRAY
    glDeleteVertexArrays(1, &_object);
#endif
}

void VertexArray::bind() {
#if !SCRAPS_EMULATE_VERTEX_ARRAY
    glBindVertexArray(_object);
#else
    for (auto& attribute : _attributes) {
        glEnableVertexAttribArray(std::get<0>(attribute));
        glBindBuffer(GL_ARRAY_BUFFER, std::get<1>(attribute));
        glVertexAttribPointer(std::get<0>(attribute), std::get<2>(attribute), std::get<3>(attribute), std::get<4>(attribute), std::get<5>(attribute), reinterpret_cast<GLvoid*>(std::get<6>(attribute)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
#endif
}

void VertexArray::unbind() {
#if !SCRAPS_EMULATE_VERTEX_ARRAY
    glBindVertexArray(0);
#else
    for (auto& attribute : _attributes) {
        glDisableVertexAttribArray(std::get<0>(attribute));
    }
#endif
}

void VertexArray::setAttribute(GLuint index, VertexBuffer& buffer, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset) {
#if !SCRAPS_EMULATE_VERTEX_ARRAY
    bind();
    buffer.bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<GLvoid*>(offset));
    unbind();
#else
    _attributes.emplace_back(index, buffer.id(), size, type, normalized, stride, offset);
#endif
}

} // namespace opengl
} // namespace scraps
