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
    virtual Shader* bind() const = 0;
};

} // namespace Hydrogen
