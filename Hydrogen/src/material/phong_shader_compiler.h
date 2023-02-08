#pragma once

#include "core.h"

#include "shader_compiler.h"
#include "renderer/texture.h"

namespace Hydrogen {

struct PhongShaderArguments {
    glm::vec3 ambient{1.0f, 1.0f, 1.0f};
    std::optional<glm::vec3> diffuse;
    std::optional<glm::vec3> specular;
    std::optional<f32> shininess;

    std::optional<Texture*> diffuse_map;
    std::optional<Texture*> specular_map;
    std::optional<Texture*> normal_map;
};

class PhongShaderCompiler : public IShaderCompiler {
  public:
    PhongShaderCompiler(PhongShaderArguments arguments);
    ~PhongShaderCompiler() = default;

    Shader* compile() const override;
    usize get_hash() const override;

  private:
    PhongShaderArguments m_arguments;
};

} // namespace Hydrogen
