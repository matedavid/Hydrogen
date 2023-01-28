#pragma once

#include "core.h"

#include "glm/glm.hpp"

namespace Hydrogen {

class HG_API Camera {
  public:
    virtual ~Camera() {}

    void set_position(const glm::vec3& pos);
    void rotate(const glm::vec2& rotation);

    const glm::vec3& get_position() const { return m_position; }

    const glm::mat4& get_view() const { return m_view; }
    const glm::mat4& get_projection() const { return m_projection; }

    glm::mat4 get_view_projection() const { return m_projection * m_view; }

  protected:
    glm::mat4 m_projection;
    glm::mat4 m_view{1.0f};

    glm::vec3 m_position{0.0f, 0.0f, 0.0f};
    glm::vec2 m_rotation{0.0f, 0.0f};

    void recalculate_view_matrix();
    virtual void recalculate_projection_matrix() = 0;
};

} // namespace Hydrogen
