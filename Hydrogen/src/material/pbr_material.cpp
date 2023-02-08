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
        .ao_map = ao_map
    });

    m_shader_id = ShaderSystem::instance->acquire_from_compiler(compiler);
    m_built = true;
}

Shader* PBRMaterial::bind() const {
    // TODO:
    HG_ASSERT(false, "Unimplemented");
    return nullptr;
}

} // namespace Hydrogen
