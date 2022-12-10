#include "orthographic_camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hydrogen {

OrthographicCamera::OrthographicCamera() {
    m_projection = glm::mat4(1.0f);
}

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) {
    m_projection = glm::ortho(left, right, bottom, top);
}

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top,
                                       float znear, float zfar) {
    m_projection = glm::ortho(left, right, bottom, top, znear, zfar);
}

}
