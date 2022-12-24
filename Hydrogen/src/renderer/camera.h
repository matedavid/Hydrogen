#pragma once

#include "core.h"

#include <glm/glm.hpp>

namespace Hydrogen {

class HG_API Camera {
  public:
    Camera();
    ~Camera() = default;

    // Orthogonal cameras
    static Camera orthogonal();
    static Camera orthogonal(float left, float right, float bottom, float top);
    static Camera orthogonal(float left, float right, float bottom, float top, float znear, float zfar);

    // Perspective camera
    static Camera perspective(float fov, float aspect, float znear, float zfar);

    void set_position(const glm::vec3& pos);
    void rotate(const glm::vec2& rotation);

    const glm::mat4& get_view() const { return m_view; }
    const glm::mat4& get_projection() const { return m_projection; }

    glm::mat4 get_view_projection() const { return m_projection * m_view; }

  protected:
    glm::mat4 m_projection;
    glm::mat4 m_view;

    glm::vec3 m_position;
    glm::vec2 m_rotation;

    Camera(const glm::mat4& proj);

    void recalculate_view_matrix();
};

} // namespace Hydrogen