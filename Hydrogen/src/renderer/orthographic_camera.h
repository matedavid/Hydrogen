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
};

} // namespace Hydrogen
