#include "renderer_api.h"

#include <glad/glad.h>

namespace Hydrogen {

void RendererAPI::clear(const glm::vec3& color) {
    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererAPI::send(const VertexArray* vao, const Shader* shader) {
    shader->bind();
    vao->bind();

    glDrawElements(GL_TRIANGLES, vao->get_count(), GL_UNSIGNED_INT, nullptr);
}

} // namespace Hydrogen
