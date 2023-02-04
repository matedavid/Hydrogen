#pragma once

#include "core.h"
#include "camera.h"

namespace Hydrogen {

class HG_API OrthographicCamera : public Camera {
  public:
    OrthographicCamera();
    OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top);
    OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar);

    ~OrthographicCamera() override = default;

  private:
    f32 m_left, m_right, m_bottom, m_top, m_znear, m_zfar;

    void recalculate_projection_matrix() override;
};

} // namespace Hydrogen