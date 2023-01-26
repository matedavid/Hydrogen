#pragma once

#include "core.h"

#include "material.h"
#include "renderer/shader.h"

namespace Hydrogen {

class ShaderCompiler {
  public:
    static Shader* from_material(const MaterialValues& material);

  private:
    static std::string BASE_VERTEX_PATH;
    static std::string BASE_FRAGMENT_PATH;
};

} // namespace Hydrogen
