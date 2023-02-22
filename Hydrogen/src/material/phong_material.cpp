#include "phong_material.h"

#include "systems/shader_system.h"
#include "phong_shader_compiler.h"

namespace Hydrogen {

PhongMaterial::PhongMaterial() : m_shader_id(), m_built(false) {
}

PhongMaterial::~PhongMaterial() {
    ShaderSystem::instance->release(m_shader_id);
}

void PhongMaterial::build() {
    if (m_built) {
        HG_LOG_WARN("You can only build a material once");
        return;
    }

    auto compiler = PhongShaderCompiler(PhongShaderArguments{
        .ambient = ambient,
        .diffuse = diffuse,
        .specular = specular,
        .shininess = shininess,
        .diffuse_map = diffuse_map,
        .specular_map = specular_map,
        .normal_map = normal_map
    });

    m_shader_id = ShaderSystem::instance->acquire_from_compiler(compiler);
    m_built = true;
}

Shader* PhongMaterial::bind(u32 slot) const {
    HG_ASSERT(m_built, "You must build the Material before binding it");

    Shader* shader = ShaderSystem::instance->get(m_shader_id);
    HG_ASSERT(shader != nullptr, "Unexpected error: shader is null");

    // Ambient Color
    shader->set_uniform_vec3("Material.ambient", ambient);

    // Diffuse Color
    if (diffuse.has_value()) {
        shader->set_uniform_vec3("Material.diffuse", diffuse.value());
    }

    // Specular color
    if (specular.has_value()) {
        shader->set_uniform_vec3("Material.specular", specular.value());
    }

    // Shininess
    if (shininess.has_value()) {
        shader->set_uniform_float("Material.shininess", shininess.value());
    }

    // Diffuse Texture
    if (diffuse_map.has_value()) {
        const Texture* diffuse_map_texture = diffuse_map.value();
        diffuse_map_texture->bind("Material.diffuse_map", shader, slot);
    }

    // Specular Texture
    if (specular_map.has_value()) {
        const Texture* specular_map_texture = specular_map.value();
        specular_map_texture->bind("Material.specular_map", shader, slot + 1);
    }

    // Normal Texture
    if (normal_map.has_value()) {
        const Texture* normal_map_texture = normal_map.value();
        normal_map_texture->bind("Material.normal_map", shader, slot + 2);
    }

    return shader;
}

} // namespace Hydrogen
