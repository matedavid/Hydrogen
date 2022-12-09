#include "vertex_array.h"

#include "glad/glad.h"

namespace Hydrogen {

VertexArray::VertexArray() {
    glGenVertexArrays(1, &ID);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &ID);

    for (const auto* vbo : m_vertex_buffers)
        delete vbo;
    delete m_index_buffer;
}

void VertexArray::bind() const {
    glBindVertexArray(ID);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

} // namespace renderer