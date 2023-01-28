#pragma once

#include "core.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "core/camera.h"
#include "vertex_array.h"
#include "buffers.h"
#include "shader.h"
#include "texture.h"

namespace Hydrogen {

class HG_API Renderer2D {
  public:
    static void init();
    static void free();

    static void begin_scene(const Camera& camera);
    static void end_scene();

    // Primitives
    static void draw_quad(const glm::vec2& pos, const glm::vec2& dim, Shader* shader);
    static void draw_quad(const glm::vec2& pos, const glm::vec2& dim, const Texture* texture);
    static void draw_quad(const glm::vec2& pos, const glm::vec2& dim, const glm::vec3& color);

  private:
    struct RendererResources {
        VertexArray* quad;

        Shader* flat_color_shader;
        Texture* white_texture;
    };
    inline static RendererResources* m_resources;

    static VertexArray* create_quad();
};

} // namespace Hydrogen