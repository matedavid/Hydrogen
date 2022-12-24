#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hydrogen {

Camera::Camera()
    : m_projection(glm::mat4(1.0f)),
      m_view(glm::mat4(1.0f)),
      m_position({0.0f, 0.0f, 0.0f}),
      m_rotation({0.0f, 0.0f}) { }

Camera Camera::orthogonal() {
    return {glm::mat4(1.0f)};
}

Camera Camera::orthogonal(float left, float right, float bottom, float top) {
    return {glm::ortho(left, right, bottom, top)};
}

Camera Camera::orthogonal(float left, float right, float bottom, float top,
                          float znear, float zfar)
{
    return {glm::ortho(left, right, bottom, top, znear, zfar)};
}

Camera Camera::perspective(float fov, float aspect, float znear, float zfar) {
    return {glm::perspective(fov, aspect, znear, zfar)};
}

void Camera::set_position(const glm::vec3& pos) {
    m_position = pos;
    recalculate_view_matrix();
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

Camera::Camera(const glm::mat4& proj)
    : m_projection(proj),
      m_view(glm::mat4(1.0f)),
      m_position({0.0f, 0.0f, 0.0f}),
      m_rotation({0.0f, 0.0f}) { }

} // namespace Hydrogen