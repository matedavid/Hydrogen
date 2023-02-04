#pragma once

#include "core.h"

#include "camera.h"

namespace Hydrogen {

class HG_API PerspectiveCamera : public Camera {
  public:
    PerspectiveCamera();
    PerspectiveCamera(f32 fov, f32 aspect, f32 znear, f32 zfar);
    ~PerspectiveCamera() = default;

    void set_fov(f32 fov) { m_fov = fov; recalculate_projection_matrix(); }
    f32 get_fov() const { return m_fov; }

    void set_aspect_ratio(f32 aspect);

  private:
    f32 m_fov, m_aspect, m_znear, m_zfar;

    void recalculate_projection_matrix() override;
};

} // namespace Hydrogen