#include "shader_compiler.h"

#include <iostream>
#include <fstream>
#include <string>

namespace Hydrogen {

std::string ShaderCompiler::BASE_VERTEX_PATH = "../../Hydrogen/assets/shaders/base.phong.vert";
std::string ShaderCompiler::BASE_FRAGMENT_PATH = "../../Hydrogen/assets/shaders/base.phong.frag";

Shader* ShaderCompiler::from_material(const MaterialValues& material) {
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

    std::string defines = "";
    if (material.diffuse.has_value()) {
        defines += "#define diffuse_color\n\n";
    }
    if (material.specular.has_value()) {
        defines += "#define specular_color\n\n";
    }
    if (material.shininess.has_value()) {
        defines += "#define shininess_def\n\n";
    }
    if (material.diffuse_map.has_value()) {
        defines += "#define diffuse_texture\n\n";
    }
    if (material.specular_map.has_value()) {
        defines += "#define specular_texture\n\n";
    }

    fragment_source = version + defines + fragment_source;

    return Shader::from_string(vertex_source, fragment_source);
}

} // namespace Hydrogen
