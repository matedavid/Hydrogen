#include "shader_system.h"

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

ShaderId ShaderSystem::acquire_from_compiler(const IShaderCompiler& compiler) {
    usize id = compiler.get_hash();

    if (m_shaders.contains(id)) {
        m_reference_count[id]++;
        return id;
    }

    HG_LOG_INFO("Loading new Shader from compiler: {}", id);

    Shader* shader = compiler.compile();
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
