#include "phong_shader_compiler.h"

#include <fstream>

namespace Hydrogen {

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
    if (m_arguments.diffuse.has_value()) {
        defines += "#define diffuse_color\n";
    }
    if (m_arguments.specular.has_value()) {
        defines += "#define specular_color\n";
    }
    if (m_arguments.shininess.has_value()) {
        defines += "#define shininess_def\n";
    }
    if (m_arguments.diffuse_map.has_value()) {
        defines += "#define diffuse_texture\n";
    }
    if (m_arguments.specular_map.has_value()) {
        defines += "#define specular_texture\n";
    }
    if (m_arguments.normal_map.has_value()) {
        defines += "#define normal_texture\n";
    }

    fragment_source = version + defines + fragment_source;

    return Shader::from_string(vertex_source, fragment_source);
}

usize PhongShaderCompiler::get_hash() const {
    // 6 optional values, using bit representation
    ShaderId diffuse_id = m_arguments.diffuse.has_value();
    ShaderId specular_id = m_arguments.specular.has_value() * (ShaderId)1e1;
    ShaderId shininess_id = m_arguments.shininess.has_value() * (ShaderId)1e2;
    ShaderId diffuse_map_id = m_arguments.diffuse_map.has_value() * (ShaderId)1e3;
    ShaderId specular_map_id = m_arguments.specular_map.has_value() * (ShaderId)1e4;
    ShaderId normal_map_id = m_arguments.normal_map.has_value() * (ShaderId)1e5;

    return diffuse_id + specular_id + shininess_id + diffuse_map_id + specular_map_id +
           normal_map_id;
}

} // namespace Hydrogen
