#pragma once

#include "core.h"

#include "glm/glm.hpp"

#include "renderer/texture.h"
#include "renderer/shader.h"

namespace Hydrogen {

using ShaderId = size_t;

// ======
template<typename T>
class Optional {
  public:
    Optional() { m_has_value = false; }
    Optional(T value) {
        m_value = value;
        m_has_value = true;
    }

    Optional& operator=(const T& value) {
        m_value = value;
        m_has_value = true;
        return *this;
    }

    const T& get() const {
        HG_ASSERT(m_has_value, "There must be a value");
        return m_value;
    }
    bool has_value() const { return m_has_value; }

  private:
    T m_value;
    bool m_has_value;
};
// ======

struct HG_API MaterialValues {
    glm::vec3 ambient{1.0f, 1.0f, 1.0f};
    Optional<glm::vec3> diffuse;
    Optional<glm::vec3> specular;
    Optional<float> shininess;

    Optional<Texture*> diffuse_map;
    Optional<Texture*> specular_map;
};

class HG_API Material {
  public:
    MaterialValues values;

    Material();
    ~Material();

    void build();
    Shader* bind() const;

  private:
    ShaderId m_shader_id;
    bool m_built;
};

} // namespace Hydrogen
