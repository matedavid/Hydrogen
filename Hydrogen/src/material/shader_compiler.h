#pragma once

#include "core.h"

#include "renderer/shader.h"

namespace Hydrogen {

class HG_API IShaderCompiler {
  public:
    virtual Shader* compile() const = 0;
    virtual usize get_hash() const = 0;
};

} // namespace Hydrogen