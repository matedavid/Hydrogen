#include "renderer3d.h"

#include <iostream>
#include "renderer_api.h"

namespace Hydrogen {

void Renderer3D::init() {
    // Rendering Resources
    m_resources = new RendererResources{};
    m_resources->quad = create_quad();

    m_resources->flat_color_shader = Shader::default_();
    m_resources->camera_ubo = new UniformBuffer(sizeof(glm::mat4)); // glm::mat4 ViewProjection matrix
    m_resources->white_texture = Texture::white();

    // Rendering Context
    m_context = new RenderingContext{};
}

void Renderer3D::free() {
    delete m_resources->quad;
    delete m_resources->flat_color_shader;
    delete m_resources->camera_ubo;
    delete m_resources->white_texture;

    delete m_resources;
    delete m_context;
}

void Renderer3D::begin_scene(const Camera& camera) {
    m_resources->camera_ubo->set_mat4(0, camera.get_view_projection());
}

void Renderer3D::end_scene() {
    // TEMPORAL: Draw lights
    for (const Light& light : m_context->lights) {
        Renderer3D::draw_cube(light.position, {0.25f, 0.25f, 0.25f}, light.diffuse);
    }
    // ======

    m_context->lights.clear();
}

void Renderer3D::add_light_source(const Light& light) {
    // TODO: Use UBO to pass along lights?
    m_context->lights.push_back(light);
}

void Renderer3D::draw_cube(const glm::vec3& pos, const glm::vec3& dim, Shader* shader) {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, dim);

    shader->set_uniform_mat4("Model", model);
    shader->assign_uniform_buffer("Camera", m_resources->camera_ubo, 0);

    RendererAPI::send(m_resources->quad, shader);
}

void Renderer3D::draw_cube(const glm::vec3& pos, const glm::vec3& dim, const Texture* texture) {
    texture->bind(0);

    m_resources->flat_color_shader->bind();
    m_resources->flat_color_shader->set_uniform_int("Texture", 0);
    m_resources->flat_color_shader->set_uniform_vec3("Color", glm::vec3(1.0f, 1.0f, 1.0f));

    Renderer3D::draw_cube(pos, dim, m_resources->flat_color_shader);
}

void Renderer3D::draw_cube(const glm::vec3& pos, const glm::vec3& dim, const glm::vec3& color) {
    m_resources->white_texture->bind(0);

    m_resources->flat_color_shader->bind();
    m_resources->flat_color_shader->set_uniform_int("Texture", 0);
    m_resources->flat_color_shader->set_uniform_vec3("Color", color);

    Renderer3D::draw_cube(pos, dim, m_resources->flat_color_shader);
}

void Renderer3D::draw_cube(const glm::vec3& pos, const glm::vec3& dim, const Material& material) {
    Shader* shader = material.bind();
    Renderer3D::draw_cube(pos, dim, shader);
}

void Renderer3D::draw_model(const Model& model, const glm::vec3& pos, const glm::vec3& dim) {
    for (const auto* mesh : model.get_meshes()) {
        VertexArray* VAO = mesh->VAO;

        Shader* shader = mesh->material.bind();
        shader->assign_uniform_buffer("Camera", m_resources->camera_ubo, 0);

        auto m = glm::mat4(1.0f);
        m = glm::translate(m, pos);
        m = glm::scale(m, dim);
        shader->set_uniform_mat4("Model", m);

        // Add point lights
        // TODO: Use UBO to pass along lights?
        shader->set_uniform_int("NumberPointLights", (int)m_context->lights.size());
        for (unsigned int i = 0; i < m_context->lights.size(); ++i) {
            const Light& light = m_context->lights[i];

            const std::string header = "PointLights[" + std::to_string(i) + "]";
            shader->set_uniform_vec3(header + ".position", light.position);

            shader->set_uniform_float(header + ".constant", light.constant);
            shader->set_uniform_float(header + ".linear", light.linear);
            shader->set_uniform_float(header + ".quadratic", light.quadratic);

            shader->set_uniform_vec3(header + ".ambient", light.ambient);
            shader->set_uniform_vec3(header + ".diffuse", light.diffuse);
            shader->set_uniform_vec3(header + ".specular", light.specular);
        }

        RendererAPI::send(VAO, shader);
    }
}

VertexArray* Renderer3D::create_quad() {
    // Create Vertex Array
    auto* vao = new VertexArray();
    vao->bind();

    // Create Vertex Buffer and Index Buffer
    float vertices[] = {
        // top face
        -0.5f, 0.5f, -0.5f,  // top left       (0)
        -0.5f, 0.5f, 0.5f,   // bottom left    (1)
        0.5f, 0.5f, 0.5f,    // bottom right   (2)
        0.5f, 0.5f, -0.5f,   // top right      (3)

        // bottom face
        -0.5f, -0.5f, -0.5f,  // top left      (4)
        -0.5f, -0.5f, 0.5f,   // bottom left   (5)
        0.5f, -0.5f, 0.5f,    // bottom right  (6)
        0.5f, -0.5f, -0.5f    // top right     (7)
    };

    unsigned int indices[] = {
        // top face
        0, 1, 3, 1, 2, 3,
        // front face
        1, 5, 2, 5, 6, 2,
        // left face
        0, 4, 1, 4, 5, 1,
        // right face
        2, 6, 7, 7, 3, 2,
        // bottom face
        4, 1, 7, 5, 6, 7
    };

    auto* vbo = new VertexBuffer(vertices, sizeof(vertices));
    vbo->set_layout({
        {.type = ShaderType::Float32, .count = 3, .normalized = false},
    });

    const auto* ebo = new IndexBuffer(indices, sizeof(indices) / sizeof(unsigned int));
    ebo->bind();

    // Add Vertex Buffer and Index Buffer to Vertex Array
    vao->add_vertex_buffer(vbo);
    vao->set_index_buffer(ebo);

    // Unbind elements
    vao->unbind();
    vbo->unbind();
    ebo->unbind();

    return vao;
}

} // namespace renderer