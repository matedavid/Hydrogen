#pragma once

#include "core.h"

#include "shader_compiler.h"
#include "renderer/texture.h"

namespace Hydrogen {

struct PBRShaderArguments {
    std::optional<glm::vec3> albedo;
    std::optional<float> metallic;
    std::optional<float> roughness;
    std::optional<float> ao;

    std::optional<Texture*> albedo_map;
    std::optional<Texture*> metallic_map;
    std::optional<Texture*> roughness_map;
    std::optional<Texture*> ao_map;
};

class HG_API PBRShaderCompiler : public IShaderCompiler {
  public:
    PBRShaderCompiler(PBRShaderArguments arguments);
    ~PBRShaderCompiler() = default;

    Shader* compile() const override;
    usize get_hash() const override;

  private:
    PBRShaderArguments m_arguments;
};

} // namespace Hydrogen
