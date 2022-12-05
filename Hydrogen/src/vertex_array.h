#pragma once

#include "core.h"

#include <vector>

#include "buffers.h"

namespace Hydrogen {

class HG_API VertexArray {
  public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;

    void add_vertex_buffer(const VertexBuffer* vbo) { m_vertex_buffers.push_back(vbo); }
    void set_index_buffer(const IndexBuffer* ebo) { m_index_buffer = ebo; }

    int get_count() const { return m_index_buffer->get_count(); }

  private:
    unsigned int ID;

    std::vector<const VertexBuffer*> m_vertex_buffers;
    const IndexBuffer* m_index_buffer;
};

} // namespace renderer