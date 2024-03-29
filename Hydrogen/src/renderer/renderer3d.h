#pragma once

#include "core.h"

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "material/material.h"
#include "core/model.h"
#include "core/mesh.h"

#include "core/camera.h"
#include "vertex_array.h"
#include "buffers.h"
#include "shader.h"
#include "texture.h"
#include "skybox.h"

namespace Hydrogen {

struct HG_API Light {
    glm::vec3 position;

    f32 constant = 1.0f;
    f32 linear = 0.09f;
    f32 quadratic = 0.032f;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class HG_API Renderer3D {
  public:
    static void init();
    static void free();

    static void begin_frame(const Camera& camera);
    static void end_frame();

    // Scene configuration
    static void add_light_source(const Light& light);
    static void set_skybox(const Skybox* skybox);

    // Primitives
    static void draw_cube(const glm::vec3& pos, const glm::vec3& dim, Shader* shader);
    static void draw_cube(const glm::vec3& pos, const glm::vec3& dim, const Texture* texture);
    static void draw_cube(const glm::vec3& pos, const glm::vec3& dim, const glm::vec3& color);
    static void draw_cube(const glm::vec3& pos, const glm::vec3& dim, const IMaterial& material);
    static void draw_sphere(const glm::vec3& pos, const glm::vec3& dim, const IMaterial& material);

    // Model
    static void draw_model(const Model& model, const glm::vec3& pos, const glm::vec3& dim);
    static void draw_model(const Model& model, const glm::vec3& pos, const glm::vec3& dim, const IMaterial& material);

  private:
    struct RendererResources {
        VertexArray* quad;
        VertexArray* sphere;

        Shader* flat_color_shader;
        Texture* white_texture;
    };
    inline static RendererResources* m_resources;

    struct RenderingContext {
        UniformBuffer* camera_ubo;
        std::vector<Light> lights;
        const Skybox* skybox = nullptr;
    };
    inline static RenderingContext* m_context;

    static VertexArray* create_quad();
    static VertexArray* create_sphere();
};

} // namespace Hydrogen