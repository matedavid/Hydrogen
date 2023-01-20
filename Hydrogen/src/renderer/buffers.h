#pragma once

#include "core.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace Hydrogen {

//
// Attribute Data
//
enum class HG_API ShaderType { Float32, UnsignedInt, Bool };

struct HG_API VertexLayout {
    ShaderType type;
    int count;
    bool normalized;
};

//
// Vertex Buffer
//
class HG_API VertexBuffer {
  public:
    VertexBuffer(const void* vertices, unsigned int size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;

    void set_layout(const std::vector<VertexLayout>& layout);

  private:
    unsigned int ID;
};

//
// Index Buffer
//
class HG_API IndexBuffer {
  public:
    IndexBuffer(const unsigned int* indices, int number_indices);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    int get_count() const { return m_count; }

  private:
    unsigned int ID;
    int m_count;
};

//
// Uniform Buffer
//
class HG_API UniformBuffer {
  public:
    UniformBuffer(unsigned int size);
    ~UniformBuffer();

    template<typename T>
    void set_data(unsigned int offset, unsigned int size, const T& data) {
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, glm::value_ptr(data));
    }

    void assign_slot(unsigned int slot) const;

    void bind() const;
    void unbind() const;

  private:
    unsigned int ID;
};

} // namespace Hydrogen