#include "camera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Hydrogen {

void Camera::set_position(const glm::vec3& pos) {
    m_position = pos;
    recalculate_view_matrix();
}

glm::vec3 Camera::get_position() const {
    glm::vec3 position = glm::vec3(m_view * glm::vec4(m_position, 1.0f));
    return position;
}

void Camera::rotate(const glm::vec2& rotation) {
    m_rotation += rotation;
    recalculate_view_matrix();
}

void Camera::recalculate_view_matrix() {
    m_view = glm::mat4(1.0f);
    m_view = glm::translate(m_view, -m_position);
    m_view = glm::rotate(m_view, m_rotation.y, glm::vec3(1.0f, 0.0f, 0.0f));
    m_view = glm::rotate(m_view, -m_rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
}

} // namespace Hydrogen