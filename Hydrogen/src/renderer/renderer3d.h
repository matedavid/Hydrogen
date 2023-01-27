#pragma once

#include "core.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "material.h"
#include "models/model.h"
#include "models/mesh.h"

#include "camera.h"
#include "vertex_array.h"
#include "buffers.h"
#include "shader.h"
#include "texture.h"

namespace Hydrogen {

struct Light {
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class HG_API Renderer3D {
  public:
    static void init();
    static void free();

    static void begin_scene(const Camera& camera);
    static void end_scene();

    // Light
    static void add_light_source(const Light& light);

    // Primitives
    static void draw_cube(const glm::vec3& pos, const glm::vec3& dim, Shader* shader);
    static void draw_cube(const glm::vec3& pos, const glm::vec3& dim, const Texture* texture);
    static void draw_cube(const glm::vec3& pos, const glm::vec3& dim, const glm::vec3& color);
    static void draw_cube(const glm::vec3& pos, const glm::vec3& dim, const Material& material);

    // Model
    static void draw_model(const Model& model, const glm::vec3& pos, const glm::vec3& dim);

  private:
    struct RendererResources {
        VertexArray* quad;

        Shader* flat_color_shader;
        Texture* white_texture;
        UniformBuffer* camera_ubo;
    };
    inline static RendererResources* m_resources;

    struct RenderingContext {
        std::vector<Light> lights;
    };
    inline static RenderingContext* m_context;

    static VertexArray* create_quad();
};

} // namespace Hydrogen