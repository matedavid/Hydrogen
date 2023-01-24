#include "material.h"

namespace Hydrogen {

Material::Material() : m_shader(nullptr) {
}

Material::~Material() {
}

void Material::build() {
    // TODO:
}

Shader* Material::get_shader() const {
    HG_ASSERT(m_shader != nullptr, "You must build the Material before getting the shader");
    return m_shader;
}

void Material::bind() const {
    HG_ASSERT(m_shader != nullptr, "You must build the Material before getting the shader");

    // Ambient Color
    m_shader->set_uniform_vec3("Material.Ambient", values.ambient);

    // Diffuse Color
    if (values.diffuse.has_value()) {
        m_shader->set_uniform_vec3("Material.Diffuse", values.diffuse.get());
    }

    // Specular color
    if (values.specular.has_value()) {
        m_shader->set_uniform_vec3("Material.Specular", values.specular.get());
    }

    // Shininess
    if (values.shininess.has_value()) {
        m_shader->set_uniform_float("Material.Shininess", values.shininess.get());
    }

    // Diffuse Texture
    if (values.diffuse_map.has_value()) {
        const Texture* diffuse_map = values.diffuse_map.get();
        diffuse_map->bind(0);
        m_shader->set_uniform_int("Material.DiffuseMap", 0);
    }

    // Specular Texture
    if (values.specular_map.has_value()) {
        const Texture* specular_map = values.specular_map.get();
        specular_map->bind(1);
        m_shader->set_uniform_int("Material.SpecularMap", 1);
    }

    m_shader->bind();
}

} // namespace Hydrogen