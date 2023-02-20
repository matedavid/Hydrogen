#include "renderer3d.h"

#include <iostream>
#include "renderer_api.h"
#include <glm/gtx/transform.hpp>
#include <cmath>

namespace Hydrogen {

void Renderer3D::init() {
    // Rendering Context
    m_context = new RenderingContext{};
    // camera_ubo = mat4 (Projection) + mat4 (View) + vec3 (which has the same size as vec4)
    m_context->camera_ubo = new UniformBuffer(2 * sizeof(glm::mat4) + sizeof(glm::vec4));

    // Rendering Resources
    m_resources = new RendererResources{};
    m_resources->quad = create_quad();
    m_resources->sphere = create_sphere();
    m_resources->flat_color_shader = Shader::default_();
    m_resources->white_texture = Texture::white();
}

void Renderer3D::free() {
    delete m_resources->quad;
    delete m_resources->sphere;
    delete m_resources->flat_color_shader;
    delete m_resources->white_texture;
    delete m_resources;

    delete m_context->camera_ubo;
    delete m_context;
}

void Renderer3D::begin_scene(const Camera& camera) {
    m_context->camera_ubo->set_mat4(0, camera.get_projection());
    m_context->camera_ubo->set_mat4(1, camera.get_view());
    m_context->camera_ubo->set_vec3(2, camera.get_position());
}

void Renderer3D::end_scene() {
    // Draw lights
    for (const Light& light : m_context->lights) {
        Renderer3D::draw_cube(light.position, {0.25f, 0.25f, 0.25f}, light.diffuse);
    }
    m_context->lights.clear();

    // Draw Skybox
    if (m_context->skybox != nullptr) {
        glDepthFunc(GL_LEQUAL);

        auto* shader = m_context->skybox->bind(0);
        Renderer3D::draw_cube({0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f}, shader);
        m_context->skybox->unbind();

        glDepthFunc(GL_LESS);
    }
}

void Renderer3D::add_light_source(const Light& light) {
    // TODO: Use UBO to pass along lights?
    m_context->lights.push_back(light);
}

void Renderer3D::set_skybox(const Skybox* skybox) {
    m_context->skybox = skybox;
}

void Renderer3D::draw_cube(const glm::vec3& pos, const glm::vec3& dim, Shader* shader) {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, dim);

    shader->set_uniform_mat4("Model", model);
    shader->assign_uniform_buffer("Camera", m_context->camera_ubo, 0);

    RendererAPI::send(m_resources->quad, shader);
}

void Renderer3D::draw_cube(const glm::vec3& pos, const glm::vec3& dim, const Texture* texture) {
    m_resources->flat_color_shader->bind();
    texture->bind("Texture", m_resources->flat_color_shader, 0);
    m_resources->flat_color_shader->set_uniform_vec3("Color", glm::vec3(1.0f, 1.0f, 1.0f));

    Renderer3D::draw_cube(pos, dim, m_resources->flat_color_shader);
}

void Renderer3D::draw_cube(const glm::vec3& pos, const glm::vec3& dim, const glm::vec3& color) {
    m_resources->flat_color_shader->bind();
    m_resources->white_texture->bind("Texture", m_resources->flat_color_shader, 0);
    m_resources->flat_color_shader->set_uniform_vec3("Color", color);

    Renderer3D::draw_cube(pos, dim, m_resources->flat_color_shader);
}

void Renderer3D::draw_cube(const glm::vec3& pos, const glm::vec3& dim, const IMaterial& material) {
    Shader* shader = material.bind();
    Renderer3D::draw_cube(pos, dim, shader);
}

void Renderer3D::draw_sphere(const glm::vec3& pos, const glm::vec3& dim, const IMaterial& material) {
    Shader* shader = material.bind();
    shader->bind();

    auto model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, dim);

    shader->set_uniform_mat4("Model", model);
    shader->assign_uniform_buffer("Camera", m_context->camera_ubo, 0);

    // Add point lights
    shader->set_uniform_int("NumberPointLights", (i32)m_context->lights.size());
    for (u32 i = 0; i < m_context->lights.size(); ++i) {
        const Light& light = m_context->lights[i];

        const std::string header = "PointLights[" + std::to_string(i) + "]";
        shader->set_uniform_vec3(header + ".position", light.position);
        shader->set_uniform_vec3(header + ".diffuse", light.diffuse);
    }

    // Add skybox
    if (m_context->skybox != nullptr) {
        m_context->skybox->bind_to_shader(shader, 10);
    }

    m_resources->sphere->bind();
    glDrawElements(GL_TRIANGLE_STRIP, m_resources->sphere->get_count(), GL_UNSIGNED_INT, 0);
}

void Renderer3D::draw_model(const Model& model, const glm::vec3& pos, const glm::vec3& dim) {
    for (const auto* mesh : model.get_meshes()) {
        VertexArray* VAO = mesh->VAO;

        Shader* shader = mesh->material->bind();
        shader->assign_uniform_buffer("Camera", m_context->camera_ubo, 0);

        auto m = glm::mat4(1.0f);
        m = glm::translate(m, pos);
        m = glm::scale(m, dim);
        shader->set_uniform_mat4("Model", m);

        // Add point lights
        // TODO: Use UBO to pass along lights?
        shader->set_uniform_int("NumberPointLights", (i32)m_context->lights.size());
        for (u32 i = 0; i < m_context->lights.size(); ++i) {
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

        // Add skybox
        if (m_context->skybox != nullptr) {
            m_context->skybox->bind_to_shader(shader, 10);
        }

        RendererAPI::send(VAO, shader);
    }
}

VertexArray* Renderer3D::create_quad() {
    // Create Vertex Array
    auto* vao = new VertexArray();
    vao->bind();

    // Create Vertex Buffer and Index Buffer
    f32 vertices[] = {
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

    u32 indices[] = {
        // top face
        0, 1, 3,
        1, 2, 3,
        // front face
        1, 5, 2,
        5, 6, 2,
        // left face
        0, 4, 1,
        4, 5, 1,
        // right face
        2, 6, 7,
        7, 3, 2,
        // bottom face
        4, 5, 7,
        5, 6, 7,
        // back face
        0, 4, 3,
        4, 7, 3
    };

    auto* vbo = new VertexBuffer(vertices, sizeof(vertices));
    vbo->set_layout({
        {.type = ShaderType::Float32, .count = 3, .normalized = false},
    });

    const auto* ebo = new IndexBuffer(indices, sizeof(indices) / sizeof(u32));
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

VertexArray* Renderer3D::create_sphere() {
    auto* vao = new VertexArray();

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<u32> indices;

    const u32 X_SEGMENTS = 64;
    const u32 Y_SEGMENTS = 64;
    const f32 PI = 3.14159265359f;

    for (u32 x = 0; x <= X_SEGMENTS; ++x) {
        for (u32 y = 0; y <= Y_SEGMENTS; ++y) {
            f32 xSegment = (f32)x / (f32)X_SEGMENTS;
            f32 ySegment = (f32)y / (f32)Y_SEGMENTS;
            f32 xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            f32 yPos = std::cos(ySegment * PI);
            f32 zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow = false;
    for (u32 y = 0; y < Y_SEGMENTS; ++y) {
        // even rows: y == 0, y == 2; and so on
        if (!oddRow) {
            for (u32 x = 0; x <= X_SEGMENTS; ++x) {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        } else {
            for (i32 x = X_SEGMENTS; x >= 0; --x) {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + (u32)x);
                indices.push_back(y * (X_SEGMENTS + 1) + (u32)x);
            }
        }
        oddRow = !oddRow;
    }

    std::vector<f32> data;
    for (u32 i = 0; i < positions.size(); ++i) {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0) {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uv.size() > 0) {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
    }

    auto* vbo = new VertexBuffer((void*)data.data(), (u32)data.size()*sizeof(float));
    vbo->set_layout({
        {.type = ShaderType::Float32, .count = 3, .normalized = false},
        {.type = ShaderType::Float32, .count = 3, .normalized = false},
        {.type = ShaderType::Float32, .count = 2, .normalized = false},
    });

    const auto* ebo = new IndexBuffer(indices.data(), (u32)indices.size());

    vao->add_vertex_buffer(vbo);
    vao->set_index_buffer(ebo);

    // Unbind elements
    vao->unbind();
    vbo->unbind();
    ebo->unbind();

    return vao;
}

} // namespace renderer