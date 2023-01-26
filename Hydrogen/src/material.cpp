#include "material.h"

#include "systems/shader_system.h"

namespace Hydrogen {

Material::Material() : m_shader_id(), m_built(false) {
}

Material::~Material() {
    ShaderSystem::instance->release(m_shader_id);
}

void Material::build() {
    if (m_built) {
        HG_LOG_WARN("You can only build a material once");
        return;
    }

    m_built = true;
    m_shader_id = ShaderSystem::instance->acquire_from_material(values);
}

Shader* Material::bind() const {
    HG_ASSERT(m_built, "You must build the Material before getting the shader");

    Shader* shader = ShaderSystem::instance->get(m_shader_id);

    // Ambient Color
    shader->set_uniform_vec3("Material.ambient", values.ambient);

    // Diffuse Color
    if (values.diffuse.has_value()) {
        shader->set_uniform_vec3("Material.diffuse", values.diffuse.get());
    }

    // Specular color
    if (values.specular.has_value()) {
        shader->set_uniform_vec3("Material.specular", values.specular.get());
    }

    // Shininess
    if (values.shininess.has_value()) {
        shader->set_uniform_float("Material.shininess", values.shininess.get());
    }

    // Diffuse Texture
    if (values.diffuse_map.has_value()) {
        const Texture* diffuse_map = values.diffuse_map.get();
        diffuse_map->bind(0);
        shader->set_uniform_int("Material.diffuse_map", 0);
    }

    // Specular Texture
    if (values.specular_map.has_value()) {
        const Texture* specular_map = values.specular_map.get();
        specular_map->bind(1);
        shader->set_uniform_int("Material.specular_map", 1);
    }

    return shader;
}

} // namespace Hydrogen