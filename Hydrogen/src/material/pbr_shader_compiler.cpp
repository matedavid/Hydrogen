#include "pbr_shader_compiler.h"

#include <fstream>

namespace Hydrogen {

#define REGISTER_DEFINE(opt, string)       \
    if (m_arguments.opt.has_value()) {     \
        defines += "#define " string "\n"; \
    }

#define REGISTER_HASH_COMPONENT(opt, result, iter)              \
    result += m_arguments.opt.has_value() * (usize)(1 << iter); \
    iter++;

// const std::string BASE_VERTEX_PATH = "shaders/base.pbr.vert";
const std::string BASE_VERTEX_PATH = "../../Hydrogen/assets/shaders/base.pbr.vert";
// const std::string BASE_FRAGMENT_PATH = "shaders/base.pbr.frag";
const std::string BASE_FRAGMENT_PATH = "../../Hydrogen/assets/shaders/base.pbr.frag";

PBRShaderCompiler::PBRShaderCompiler(PBRShaderArguments arguments) : m_arguments(arguments) {
}

Shader* PBRShaderCompiler::compile() const {
    // TODO: Move to FileSystem module (once it's implemented)
    std::ifstream vertex_file(BASE_VERTEX_PATH);
    HG_ASSERT(vertex_file.is_open(), "Could not open vertex shader file: {}", BASE_VERTEX_PATH);
    std::string vertex_source((std::istreambuf_iterator<char>(vertex_file)),
                              std::istreambuf_iterator<char>());

    std::ifstream fragment_file(BASE_FRAGMENT_PATH);
    HG_ASSERT(fragment_file.is_open(), "Could not open fragment shader file: {}",
              BASE_FRAGMENT_PATH);
    std::string fragment_source((std::istreambuf_iterator<char>(fragment_file)),
                                std::istreambuf_iterator<char>());

    vertex_file.close();
    fragment_file.close();
    // ======

    const std::string version = "#version 330 core\n\n";

    std::string defines;
    REGISTER_DEFINE(albedo, "albedo_color");
    REGISTER_DEFINE(metallic, "metallic_value");
    REGISTER_DEFINE(roughness, "roughness_value");
    REGISTER_DEFINE(ao, "ao_value");
    REGISTER_DEFINE(albedo_map, "albedo_texture");
    REGISTER_DEFINE(metallic_map, "metallic_texture");
    REGISTER_DEFINE(roughness_map, "roughness_texture");
    REGISTER_DEFINE(ao_map, "ao_texture");

    fragment_source = version + defines + fragment_source;

    return Shader::from_string(vertex_source, fragment_source);
}

usize PBRShaderCompiler::get_hash() const {
    usize hash = 0;
    u32 iter = 1;
    REGISTER_HASH_COMPONENT(albedo, hash, iter);
    REGISTER_HASH_COMPONENT(metallic, hash, iter);
    REGISTER_HASH_COMPONENT(roughness, hash, iter);
    REGISTER_HASH_COMPONENT(ao, hash, iter);
    REGISTER_HASH_COMPONENT(albedo_map, hash, iter);
    REGISTER_HASH_COMPONENT(metallic_map, hash, iter);
    REGISTER_HASH_COMPONENT(roughness_map, hash, iter);
    REGISTER_HASH_COMPONENT(ao_map, hash, iter);

    return (hash * 0x08475) % 43476;
}

} // namespace Hydrogen