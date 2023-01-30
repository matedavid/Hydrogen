#pragma once

#include "core.h"

#include "camera.h"

namespace Hydrogen {

class HG_API PerspectiveCamera : public Camera {
  public:
    PerspectiveCamera();
    PerspectiveCamera(float fov, float aspect, float znear, float zfar);
    ~PerspectiveCamera() = default;

    void set_fov(float fov) { m_fov = fov; recalculate_projection_matrix(); }
    float get_fov() const { return m_fov; }

    void set_aspect_ratio(float aspect) { m_aspect = aspect; recalculate_projection_matrix(); }

  private:
    float m_fov, m_aspect, m_znear, m_zfar;

    void recalculate_projection_matrix() override;
};

} // namespace Hydrogen