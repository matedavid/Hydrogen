#pragma once

#include "core.h"

#include "material.h"

namespace Hydrogen {

class HG_API PhongMaterial : public IMaterial {
  public:
    PhongMaterial();
    ~PhongMaterial();

    void build() override;
    Shader* bind() const override;

  private:
    ShaderId m_shader_id;
    bool m_built;

  public:
    // Material values
    glm::vec3 ambient{1.0f, 1.0f, 1.0f};
    std::optional<glm::vec3> diffuse;
    std::optional<glm::vec3> specular;
    std::optional<f32> shininess;

    std::optional<Texture*> diffuse_map;
    std::optional<Texture*> specular_map;
    std::optional<Texture*> normal_map;
};

} // namespace Hydrogen
