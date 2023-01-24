#include "shader_system.h"

namespace Hydrogen {

ShaderSystem* ShaderSystem::instance = nullptr;

void ShaderSystem::init() {
    HG_ASSERT(instance == nullptr, "You can only initialize ShaderSystem once");
    instance = new ShaderSystem();
}

void ShaderSystem::destroy() {
    HG_ASSERT(instance != nullptr, "You must initialize ShaderSystem before it's destroyed");
    delete instance;
}

ShaderSystem::ShaderSystem() {
    // TODO:
}

ShaderSystem::~ShaderSystem() {
    for (auto value : m_shaders) {
        delete value.second;
    }
}

ShaderId ShaderSystem::acquire_from_source(const std::string& vertex_src,
                                           const std::string& fragment_src) {
    // TODO:
}

ShaderId ShaderSystem::acquire_from_file(const std::string& vertex_path,
                                         const std::string& fragment_path) {
    // TODO:
}

ShaderId ShaderSystem::acquire_from_material(const MaterialValues& material) {
    // TODO:
}

} // namespace Hydrogen
