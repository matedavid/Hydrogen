#pragma once

#include <vector>

namespace Hydrogen {

//
// Attribute Data
//
enum class ShaderType { Float32, UnsignedInt, Bool };

struct VertexLayout {
    ShaderType type;
    int count;
    bool normalized;
};

//
// Vertex Buffer
//
class VertexBuffer {
  public:
    VertexBuffer(const float* vertices, unsigned int size);
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
class IndexBuffer {
  public:
    IndexBuffer(const unsigned int* indices, int count);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    int get_count() const { return m_count; }

  private:
    unsigned int ID;
    int m_count;
};

} // namespace renderer