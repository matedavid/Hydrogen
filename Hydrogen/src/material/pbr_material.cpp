#include "pbr_material.h"

#include "systems/shader_system.h"
#include "pbr_shader_compiler.h"

namespace Hydrogen {

PBRMaterial::PBRMaterial() : m_shader_id(), m_built(false) {
}

PBRMaterial::~PBRMaterial() {
    ShaderSystem::instance->release(m_shader_id);
}

void PBRMaterial::build() {
    if (m_built) {
        HG_LOG_WARN("You can only build a material once");
        return;
    }

    auto compiler = PBRShaderCompiler(PBRShaderArguments{
        .albedo = albedo,
        .metallic = metallic,
        .roughness = roughness,
        .ao = ao,
        .albedo_map = albedo_map,
        .metallic_map = metallic_map,
        .roughness_map = roughness_map,
        .ao_map = ao_map,
        .normal_map = normal_map,

        .metallic_roughness_same_texture = metallic_roughness_same_texture,
        .metallic_roughness_ao_same_texture = metallic_roughness_ao_same_texture
    });

    m_shader_id = ShaderSystem::instance->acquire_from_compiler(compiler);
    m_built = true;
}

Shader* PBRMaterial::bind(u32 slot) const {
    HG_ASSERT(m_built, "You must build the Material before binding it");

    auto* shader = ShaderSystem::instance->get(m_shader_id);
    HG_ASSERT(shader != nullptr, "Unexpected error: shader is null");

    // Albedo color
    if (albedo.has_value()) {
        shader->set_uniform_vec3("Material.albedo", albedo.value());
    }

    // Metallic value
    if (metallic.has_value()) {
        shader->set_uniform_float("Material.metallic", metallic.value());
    }

    // Roughness value
    if (roughness.has_value()) {
        shader->set_uniform_float("Material.roughness", roughness.value());
    }

    // AO value
    if (ao.has_value()) {
        shader->set_uniform_float("Material.ao", ao.value());
    }

    // Albedo map
    if (albedo_map.has_value()) {
        const Texture* albedo_map_texture = albedo_map.value();
        albedo_map_texture->bind("Material.albedo_map", shader, slot);
    }

    // Metallic map
    if (metallic_map.has_value()) {
        const Texture* metallic_map_texture = metallic_map.value();
        metallic_map_texture->bind("Material.metallic_map", shader, slot + 1);
    }

    // Roughness map
    if (roughness_map.has_value()) {
        const Texture* roughness_map_texture = roughness_map.value();
        roughness_map_texture->bind("Material.roughness_map", shader, slot + 2);
    }

    // AO map
    if (ao_map.has_value()) {
        const Texture* ao_map_texture = ao_map.value();
        ao_map_texture->bind("Material.ao_map", shader, slot + 3);
    }

    // Normal map
    if (normal_map.has_value()) {
        const Texture* normal_map_texture = normal_map.value();
        normal_map_texture->bind("Material.normal_map", shader, slot + 4);
    }

    return shader;
}

} // namespace Hydrogen
