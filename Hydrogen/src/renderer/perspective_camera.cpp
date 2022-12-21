#include "perspective_camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hydrogen {

PerspectiveCamera::PerspectiveCamera() {
    m_projection = glm::mat4(1.0f);
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float znear, float zfar) {
    m_projection = glm::perspective(fov, aspect, znear, zfar);
}

} // namespace Hydrogen
