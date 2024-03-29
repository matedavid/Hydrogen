#include "orthographic_camera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Hydrogen {

OrthographicCamera::OrthographicCamera() {
    m_projection = glm::mat4(1.0f);
}

OrthographicCamera::OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top)
    : m_left(left), m_right(right), m_bottom(bottom), m_top(top), m_znear(0.0f), m_zfar(0.0f)
{
    m_projection = glm::ortho(m_left, m_right, m_bottom, m_top);
}

OrthographicCamera::OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar)
    : m_left(left), m_right(right), m_bottom(bottom), m_top(top), m_znear(znear), m_zfar(zfar)
{
    m_projection = glm::ortho(m_left, m_right, m_bottom, m_top, m_znear, m_zfar);
}

void OrthographicCamera::recalculate_projection_matrix() {
    m_projection = glm::ortho(m_left, m_right, m_bottom, m_top, m_znear, m_zfar);
}

} // namespace Hydrogen