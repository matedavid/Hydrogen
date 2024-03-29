#include "perspective_camera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Hydrogen {

PerspectiveCamera::PerspectiveCamera() {
    m_projection = glm::mat4(1.0f);
}

PerspectiveCamera::PerspectiveCamera(f32 fov, f32 aspect, f32 znear, f32 zfar)
    : m_fov(fov), m_aspect(aspect), m_znear(znear), m_zfar(zfar) {
    m_projection = glm::perspective(m_fov, m_aspect, m_znear, m_zfar);
}

void PerspectiveCamera::set_aspect_ratio(f32 aspect) {
    m_aspect = aspect;

    f32 tmp_fov = m_fov;
    if (aspect < 1.0f) {
        m_fov = 2.0f * glm::atan(glm::tan(m_fov * 0.5f) / aspect);
    }
    recalculate_projection_matrix();
    m_fov = tmp_fov;
}

void PerspectiveCamera::recalculate_projection_matrix() {
    m_projection = glm::perspective(m_fov, m_aspect, m_znear, m_zfar);
}

} // namespace Hydrogen