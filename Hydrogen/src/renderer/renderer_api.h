#pragma once

#include "core.h"

#include <glm/glm.hpp>

#include "vertex_array.h"
#include "shader.h"

namespace Hydrogen {

class RendererAPI {
  public:
    static bool init(void* loader);
    static void resize(int width, int height);
    static void clear(const glm::vec3& color);
    static void send(const VertexArray* vao, const Shader* shader);
};

} // namespace Hydrogen
