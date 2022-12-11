#include "renderer3d.h"

#include <iostream>

namespace Hydrogen {

void Renderer3D::init() {
    m_resources = new RendererResources();
    m_resources->quad = create_quad();
    m_resources->flat_color_shader
        = Shader::from_file("../../Hydrogen/assets/vertex.glsl", "../../Hydrogen/assets/fragment.glsl");
}

void Renderer3D::free() {
    delete m_resources->quad;
    delete m_resources;
}

void Renderer3D::clear(const glm::vec3& color) {
    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer3D::begin_scene(const Camera& camera) {
    m_resources->flat_color_shader->bind();
    m_resources->flat_color_shader->set_uniform_mat4(camera.get_view_projection(), "ViewProjection");
}

void Renderer3D::end_scene() {
}

void Renderer3D::draw(const VertexArray* vao, const Shader* shader) {
    shader->bind();
    vao->bind();

    glDrawElements(GL_TRIANGLES, vao->get_count(), GL_UNSIGNED_INT, nullptr);
}

void Renderer3D::draw_cube(const glm::vec3& pos, const glm::vec3& dim, Shader* shader) {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
    model = glm::scale(model, glm::vec3(dim.x, dim.y, dim.z));

    shader->set_uniform_mat4(model, "Model");
    Renderer3D::draw(m_resources->quad, shader);
}

void Renderer3D::draw_cube(const glm::vec3& pos, const glm::vec3& dim, const glm::vec3& color) {
    m_resources->flat_color_shader->set_uniform_vec3(color, "Color");
    Renderer3D::draw_cube(pos, dim, m_resources->flat_color_shader);
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
    vbo->set_layout({{.type = ShaderType::Float32, .count = 3, .normalized = false}});

    const auto* ebo = new IndexBuffer(indices, sizeof(indices));
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