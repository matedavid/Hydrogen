#pragma once

#include "core.h"
#include "camera.h"

namespace Hydrogen {

class HG_API OrthographicCamera : public Camera {
  public:
    OrthographicCamera();
    OrthographicCamera(float left, float right, float bottom, float top);
    OrthographicCamera(float left, float right, float bottom, float top, float znear, float zfar);

    ~OrthographicCamera() override = default;

  private:
    float m_left, m_right, m_bottom, m_top, m_znear, m_zfar;

    void recalculate_projection_matrix() override;
};

} // namespace Hydrogen