#pragma once

#include "core.h"

#include <glm/glm.hpp>

#include "renderer/texture.h"

namespace Hydrogen {

struct HG_API Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    Texture* diffuse_map  = nullptr;
    Texture* specular_map = nullptr;
};

} // namespace Hydrogen
