#pragma once

#include "core.h"

#include <glm/glm.hpp>
#include <array>

namespace Hydrogen {

// Attribute Data
enum class HG_API ShaderType { Float32, UnsignedInt, Bool };

struct HG_API VertexLayout {
    ShaderType type;
    u32 count;
    bool normalized;
};

//
// Vertex Buffer
//
class HG_API VertexBuffer {
  public:
    VertexBuffer(const void* vertices, u32 size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;

    void set_layout(const std::vector<VertexLayout>& layout) const;

  private:
    u32 ID;
};

//
// Index Buffer
//
class HG_API IndexBuffer {
  public:
    IndexBuffer(const u32* indices, u32 number_indices);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    u32 get_count() const { return m_count; }

  private:
    u32 ID;
    u32 m_count;
};

//
// Uniform Buffer
//
class HG_API UniformBuffer {
  public:
    UniformBuffer(u32 size);
    ~UniformBuffer();

    void assign_slot(u32 slot);

    void bind() const;
    void unbind() const;

    void set_vec3(u32 pos, const glm::vec3& data);
    void set_vec4(u32 pos, const glm::vec4& data);
    void set_mat4(u32 pos, const glm::mat4& data);

  private:
    u32 ID;
    u32 current_slot;

    #define MAX_UNIFORM_POSITIONS 10
    std::array<i32, MAX_UNIFORM_POSITIONS> m_position_offset;

    template <typename T>
    void set_data(u32 pos, i32 size, const T& data);
};

} // namespace Hydrogen