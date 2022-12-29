#include "perspective_camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hydrogen {

PerspectiveCamera::PerspectiveCamera() {
    m_projection = glm::mat4(1.0f);
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float znear, float zfar)
    : m_fov(fov), m_aspect(aspect), m_znear(znear), m_zfar(zfar)
{
    m_projection = glm::perspective(m_fov, m_aspect, m_znear, m_zfar);
}

void PerspectiveCamera::recalculate_projection_matrix() {
    m_projection = glm::perspective(m_fov, m_aspect, m_znear, m_zfar);
}

} // namespace Hydrogen