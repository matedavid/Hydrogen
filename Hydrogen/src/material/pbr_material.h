#pragma once

#include "core.h"

#include "material.h"

namespace Hydrogen {

class HG_API PBRMaterial : public IMaterial {
  public:
    PBRMaterial();
    ~PBRMaterial() override;

    void build() override;
    Shader* bind(u32 slot) const override;

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
    std::optional<Texture*> normal_map;

    bool metallic_roughness_same_texture = false;
    bool metallic_roughness_ao_same_texture = false;
};

} // namespace Hydrogen
