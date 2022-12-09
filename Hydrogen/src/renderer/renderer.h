#pragma once

#include "core.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "vertex_array.h"
#include "buffers.h"
#include "shader.h"

namespace Hydrogen {

class HG_API Renderer {
  public:
    static GLFWwindow* init(int width, int height, const std::string& window_title);
    static void free(GLFWwindow* window);

    static void clear(const glm::vec3& color);

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

}