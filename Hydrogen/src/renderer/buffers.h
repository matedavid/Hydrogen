#pragma once

#include "core.h"

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

} // namespace Hydrogen