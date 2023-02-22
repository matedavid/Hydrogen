#pragma once

#include "core.h"

#include "glm/glm.hpp"

#include "renderer/texture.h"
#include "renderer/shader.h"

namespace Hydrogen {

using ShaderId = usize;

class IMaterial {
  public:
    virtual ~IMaterial() = default;

    virtual void build() = 0;
    virtual Shader* bind(u32 slot = 0) const = 0;
};

} // namespace Hydrogen
