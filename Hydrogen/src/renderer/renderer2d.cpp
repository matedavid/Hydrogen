#include "renderer2d.h"

#include "renderer_api.h"

namespace Hydrogen {

void Renderer2D::init() {
    m_resources = new RendererResources();
    m_resources->quad = create_quad();
    //m_resources->flat_color_shader = Shader::from_file("assets/vertex.glsl", "assets/fragment.glsl");
    m_resources->flat_color_shader = Shader::default_();

    m_resources->white_texture = Texture::white();
}

void Renderer2D::free() {
    delete m_resources->quad;
    delete m_resources;
}

void Renderer2D::begin_scene(const Camera& camera) {
    m_resources->flat_color_shader->bind();
    m_resources->flat_color_shader->set_uniform_mat4("ViewProjection", camera.get_view_projection());
}

void Renderer2D::end_scene() {
}

void Renderer2D::draw_quad(const glm::vec2& pos, const glm::vec2& dim, Shader* shader) {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
    model = glm::scale(model, glm::vec3(dim.x, dim.y, 1.0f));

    shader->set_uniform_mat4("Model", model);
    RendererAPI::send(m_resources->quad, shader);
}

void Renderer2D::draw_quad(const glm::vec2& pos, const glm::vec2& dim, const Texture* texture) {
    texture->bind(0);
    m_resources->flat_color_shader->set_uniform_int("Texture", 0);
    m_resources->flat_color_shader->set_uniform_vec3("Color", glm::vec3(1.0f, 1.0f, 1.0f));
    Renderer2D::draw_quad(pos, dim, m_resources->flat_color_shader);
}

void Renderer2D::draw_quad(const glm::vec2& pos, const glm::vec2& dim, const glm::vec3& color) {
    m_resources->white_texture->bind(0);
    m_resources->flat_color_shader->set_uniform_int("Texture", 0);
    m_resources->flat_color_shader->set_uniform_vec3("Color", color);
    Renderer2D::draw_quad(pos, dim, m_resources->flat_color_shader);
}

VertexArray* Renderer2D::create_quad() {
    // Create Vertex Array
    auto* vao = new VertexArray();
    vao->bind();

    // Create Vertex Buffer and Index Buffer
    float vertices[] = {
        //     Position    ||  Texture Coords
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f
    };

    unsigned int indices[] = {0, 1, 3, 2, 3, 1};

    auto* vbo = new VertexBuffer(vertices, sizeof(vertices));
    vbo->set_layout({
        {.type = ShaderType::Float32, .count = 3, .normalized = false},
        {.type = ShaderType::Float32, .count = 2, .normalized = false}
    });

    const auto* ebo = new IndexBuffer(indices, 6);
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

} // namespace Hydrogen