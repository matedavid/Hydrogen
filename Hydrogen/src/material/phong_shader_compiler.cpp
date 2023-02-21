#include "phong_shader_compiler.h"

#include <fstream>

namespace Hydrogen {

#define REGISTER_DEFINE(opt, string)       \
    if (m_arguments.opt.has_value()) {     \
        defines += "#define " string "\n"; \
    }

#define REGISTER_HASH_COMPONENT(opt, result, iter)              \
    result += m_arguments.opt.has_value() * (usize)(1 << iter); \
    iter++;

// const std::string BASE_VERTEX_PATH = "shaders/base.phong.vert";
const std::string BASE_VERTEX_PATH = "../../Hydrogen/assets/shaders/base.phong.vert";
// const std::string BASE_FRAGMENT_PATH = "shaders/base.phong.frag";
const std::string BASE_FRAGMENT_PATH = "../../Hydrogen/assets/shaders/base.phong.frag";

PhongShaderCompiler::PhongShaderCompiler(PhongShaderArguments arguments) : m_arguments(arguments) {
}

Shader* PhongShaderCompiler::compile() const {
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
    REGISTER_DEFINE(diffuse, "diffuse_color");
    REGISTER_DEFINE(specular, "specular_color");
    REGISTER_DEFINE(shininess, "shininess_def");
    REGISTER_DEFINE(diffuse_map, "diffuse_texture");
    REGISTER_DEFINE(specular_map, "specular_texture");
    REGISTER_DEFINE(normal_map, "normal_texture");

    fragment_source = version + defines + fragment_source;

    return Shader::from_string(vertex_source, fragment_source);
}

usize PhongShaderCompiler::get_hash() const {
    usize hash = 0;
    u32 iter = 1;
    REGISTER_HASH_COMPONENT(diffuse, hash, iter);
    REGISTER_HASH_COMPONENT(specular, hash, iter);
    REGISTER_HASH_COMPONENT(shininess, hash, iter);
    REGISTER_HASH_COMPONENT(diffuse_map, hash, iter);
    REGISTER_HASH_COMPONENT(specular_map, hash, iter);
    REGISTER_HASH_COMPONENT(normal_map, hash, iter);

    return ((hash << 45) * 0x14573) % 62819;
}

} // namespace Hydrogen
