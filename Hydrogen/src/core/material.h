#pragma once

#include "core.h"

#include <optional>

#include "glm/glm.hpp"

#include "renderer/texture.h"
#include "renderer/shader.h"

namespace Hydrogen {

using ShaderId = size_t;

struct HG_API MaterialValues {
    glm::vec3 ambient{1.0f, 1.0f, 1.0f};
    std::optional<glm::vec3> diffuse;
    std::optional<glm::vec3> specular;
    std::optional<float> shininess;

    std::optional<Texture*> diffuse_map;
    std::optional<Texture*> specular_map;
    std::optional<Texture*> normal_map;
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
