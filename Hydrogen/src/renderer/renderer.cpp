#include "renderer.h"

#include <iostream>

namespace Hydrogen {

GLFWwindow* Renderer::init(int width, int height, const std::string& window_title) {
    if (!glfwInit()) {
        std::cerr << "Error initializing glfw\n";
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, window_title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Error creating window\n";
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return nullptr;
    }

    // Set VSync
    glfwSwapInterval(1);
    glViewport(0, 0, width, height);

    m_resources = new RendererResources();
    m_resources->quad = create_quad();
    m_resources->flat_color_shader
        = Shader::from_file("../../Hydrogen/assets/vertex.glsl", "../../Hydrogen/assets/fragment.glsl");

    return window;
}

void Renderer::free(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();

    delete m_resources->quad;
    delete m_resources;
}

void Renderer::clear(const glm::vec3& color) {
    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::draw(const VertexArray* vao, const Shader* shader) {
    shader->bind();
    vao->bind();

    glDrawElements(GL_TRIANGLES, vao->get_count(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::draw_quad(float x, float y, float width, float height, Shader* shader) {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    shader->set_uniform_mat4(model, "Model");
    Renderer::draw(m_resources->quad, shader);
}

void Renderer::draw_quad(float x, float y, float width, float height, const glm::vec3& color) {
    m_resources->flat_color_shader->set_uniform_vec3(color, "Color");
    Renderer::draw_quad(x, y, width, height, m_resources->flat_color_shader);
}

VertexArray* Renderer::create_quad() {
    // Create Vertex Array
    auto* vao = new VertexArray();
    vao->bind();

    // Create Vertex Buffer and Index Buffer
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
    };

    unsigned int indices[] = {
        0, 1, 3,
        2, 3, 1
    };

    auto* vbo = new VertexBuffer(vertices, sizeof(vertices));
    vbo->set_layout({{ .type = ShaderType::Float32, .count = 3, .normalized = false }});

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

} // namespace renderer