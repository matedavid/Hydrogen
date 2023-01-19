#include "buffers.h"

#include <glad/glad.h>

namespace Hydrogen {

//
// Attribute Data
//
static unsigned int get_opengl_type(const ShaderType& type) {
    switch (type) {
        case ShaderType::Float32:
            return GL_FLOAT;
        case ShaderType::UnsignedInt:
            return GL_UNSIGNED_INT;
        case ShaderType::Bool:
            return GL_BOOL;
    }
    return 0;
}

static int get_type_size(const ShaderType& type) {
    switch (type) {
        case ShaderType::Float32:
        case ShaderType::UnsignedInt:
            return 4;
        case ShaderType::Bool:
            return 1;
    }
    return 0;
}

//
// Vertex Buffer
//
VertexBuffer::VertexBuffer(const void* vertices, unsigned int size) {
    glGenBuffers(1, &ID);
    bind();

    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &ID);
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::set_layout(const std::vector<VertexLayout>& layout) {
    bind();

    int generic_stride = 0;
    for (auto element : layout) {
        generic_stride += element.count * get_type_size(element.type);
    }

    int stride = 0;
    for (unsigned int i = 0; i < layout.size(); ++i) {
        auto element = layout[i];
        glVertexAttribPointer(i, element.count, get_opengl_type(element.type),
                              element.normalized ? GL_TRUE : GL_FALSE, generic_stride,
                              reinterpret_cast<const void*>(stride));
        glEnableVertexAttribArray(i);

        stride += element.count * get_type_size(element.type);
    }
}

//
// Index Buffer
//
IndexBuffer::IndexBuffer(const unsigned int* indices, int number_indices) : m_count(number_indices) {
    glGenBuffers(1, &ID);
    bind();

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, number_indices * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &ID);
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void IndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//
// Uniform Buffer
//
UniformBuffer::UniformBuffer(unsigned int size) {
    glGenBuffers(1, &ID);

    bind();
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STREAM_DRAW);

    // TODO: Make this configurable
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ID);
}

UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &ID);
}

//template <typename T>
//void UniformBuffer::set_data(unsigned int offset, const T& data) {
//    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T), glm::value_ptr(data));
//}

void UniformBuffer::bind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, ID);
}

void UniformBuffer::unbind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

} // namespace renderer