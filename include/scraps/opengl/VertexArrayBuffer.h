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
