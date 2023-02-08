#pragma once

#include "core.h"

#include "material.h"

namespace Hydrogen {

class HG_API PBRMaterial : public IMaterial {
  public:
    PBRMaterial();
    ~PBRMaterial() override;

    void build() override;
    Shader* bind() const override;

  private:
    ShaderId m_shader_id;
    bool m_built;

  public:
    // Material values
    std::optional<glm::vec3> albedo;
    std::optional<float> metallic;
    std::optional<float> roughness;
    std::optional<float> ao;

    std::optional<Texture*> albedo_map;
    std::optional<Texture*> metallic_map;
    std::optional<Texture*> roughness_map;
    std::optional<Texture*> ao_map;
};

} // namespace Hydrogen
