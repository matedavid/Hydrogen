#include "shader_system.h"

#include "shader_compiler.h"

namespace Hydrogen {

ShaderSystem* ShaderSystem::instance = nullptr;

void ShaderSystem::init() {
    HG_ASSERT(instance == nullptr, "You can only initialize ShaderSystem once");
    instance = new ShaderSystem();
}

void ShaderSystem::free() {
    HG_ASSERT(instance != nullptr, "You must initialize ShaderSystem before it's destroyed");
    delete instance;
}

ShaderSystem::ShaderSystem() {
    // TODO: Maybe include default shaders, maybe nothing
}

ShaderSystem::~ShaderSystem() {
    for (auto value : m_shaders) {
        delete value.second;
    }
}

Shader* ShaderSystem::get(ShaderId id) {
    HG_ASSERT(m_shaders.contains(id), "Shader with id: {} is not registered in ShaderSystem", id);
    return m_shaders[id];
}

ShaderId ShaderSystem::acquire_from_source(const std::string& vertex_src,
                                           const std::string& fragment_src) {
    std::hash<std::string> string_hasher;

    ShaderId vertex_hash = string_hasher(vertex_src);
    ShaderId fragment_hash = string_hasher(fragment_src);

    ShaderId id = hash_combine(vertex_hash, fragment_hash);

    if (m_shaders.contains(id)) {
        m_reference_count[id]++;
        return id;
    }

    HG_LOG_INFO("Loading new Shader from source");

    Shader* shader = Shader::from_string(vertex_src, fragment_src);
    m_reference_count[id] = 1;
    m_shaders[id] = shader;

    return id;
}

ShaderId ShaderSystem::acquire_from_file(const std::string& vertex_path,
                                         const std::string& fragment_path) {
    std::hash<std::string> string_hasher;

    ShaderId vertex_hash = string_hasher(vertex_path);
    ShaderId fragment_hash = string_hasher(fragment_path);

    ShaderId id = hash_combine(vertex_hash, fragment_hash);

    if (m_shaders.contains(id)) {
        m_reference_count[id]++;
        return id;
    }

    HG_LOG_INFO("Loading new Shader from path: {} {}", vertex_path, fragment_path);

    Shader* shader = Shader::from_file(vertex_path, fragment_path);
    m_reference_count[id] = 1;
    m_shaders[id] = shader;

    return id;
}

ShaderId ShaderSystem::acquire_from_material(const MaterialValues& material) {
    // NOTE: Only implementing for Materials using Phong model

    // 6 optional values, using bit representation
    ShaderId diffuse_id = material.diffuse.has_value();
    ShaderId specular_id = material.specular.has_value() * (ShaderId)1e1;
    ShaderId shininess_id = material.shininess.has_value() * (ShaderId)1e2;
    ShaderId diffuse_map_id = material.diffuse_map.has_value() * (ShaderId)1e3;
    ShaderId specular_map_id = material.specular_map.has_value() * (ShaderId)1e4;
    ShaderId normal_map_id = material.normal_map.has_value() * (ShaderId)1e5;

    ShaderId id =
        diffuse_id + specular_id + shininess_id + diffuse_map_id + specular_map_id + normal_map_id;

    if (m_shaders.contains(id)) {
        m_reference_count[id]++;
        return id;
    }

    HG_LOG_INFO("Loading new Shader from material: {}", id);

    Shader* shader = ShaderCompiler::from_material(material);
    m_reference_count[id] = 1;
    m_shaders[id] = shader;

    return id;
}

void ShaderSystem::release(ShaderId id) {
    if (!m_shaders.contains(id)) {
        HG_LOG_WARN("Shader with id: {} is not registered in ShaderSystem", id);
        return;
    }

    if (--m_reference_count[id] == 0) {
        HG_LOG_INFO("Releasing Shader with id: {}", id);

        delete m_shaders[id];
        m_shaders.erase(id);
        m_reference_count.erase(id);
    }
}

} // namespace Hydrogen
