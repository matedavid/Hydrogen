#pragma once

#include "core.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "camera.h"
#include "vertex_array.h"
#include "buffers.h"
#include "shader.h"

namespace Hydrogen {

class HG_API Renderer {
  public:
    static void init();
    static void free();

    static void clear(const glm::vec3& color);
    static void begin_scene(const Camera& camera);
    static void end_scene();

    static void draw(const VertexArray* vao, const Shader* shader);

    // Primitives
    static void draw_quad(float x, float y, float width, float height, Shader* shader);
    static void draw_quad(float x, float y, float width, float height, const glm::vec3& color);

  private:
    struct RendererResources {
        VertexArray* quad;

        Shader* flat_color_shader;
    };
    inline static RendererResources* m_resources;

    static VertexArray* create_quad();
};

} // namespace Hydrogen