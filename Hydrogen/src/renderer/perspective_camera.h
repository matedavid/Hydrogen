#pragma once

#include "core.h"

#include "camera.h"

namespace Hydrogen {

class HG_API PerspectiveCamera : public Camera {
  public:
    PerspectiveCamera();
    PerspectiveCamera(float fov, float aspect, float znear, float zfar);
    ~PerspectiveCamera() = default;
};

} // namespace Hydrogen
