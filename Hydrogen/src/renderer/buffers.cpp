#include "buffers.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

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
}

UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &ID);
}

void UniformBuffer::assign_slot(unsigned int slot) const {
    glBindBufferBase(GL_UNIFORM_BUFFER, slot, ID);
}

void UniformBuffer::bind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, ID);
}

void UniformBuffer::unbind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::set_vec3(unsigned int pos, const glm::vec3& data) {
    set_data(pos, 16, data);
}

void UniformBuffer::set_vec4(unsigned int pos, const glm::vec4& data) {
    set_data(pos, 16, data);
}

void UniformBuffer::set_mat4(unsigned int pos, const glm::mat4& data) {
    set_data(pos, sizeof(glm::mat4), data);
}

template <typename T>
void UniformBuffer::set_data(unsigned int pos, int size, const T& data) {
    HG_ASSERT(pos < MAX_UNIFORM_POSITIONS, "You can't use more than {} uniform positions", MAX_UNIFORM_POSITIONS);

    int offset = pos == 0 ? 0 : m_position_offset[pos-1];
    m_position_offset[pos] = offset + size;

    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, glm::value_ptr(data));
}


} // namespace renderer