#include "shader.h"

#include "glad/glad.h"

#include <cassert>
#include <fstream>

namespace Hydrogen {

Shader* Shader::from_string(const std::string& vertex_src, const std::string& fragment_src) {
    // Compile Shaders
    unsigned int vertex_shader = Shader::compile(vertex_src, GL_VERTEX_SHADER);
    unsigned int fragment_shader = Shader::compile(fragment_src, GL_FRAGMENT_SHADER);

    // Shader program
    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    // Cleanup
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return new Shader(shader_program);
}

Shader* Shader::from_file(const std::string& vertex_path, const std::string& fragment_path) {
    std::ifstream vertex_file(vertex_path);
    assert(vertex_file.is_open());
    std::string vertex_source((std::istreambuf_iterator<char>(vertex_file)),
                              std::istreambuf_iterator<char>());

    std::ifstream fragment_file(fragment_path);
    assert(fragment_file.is_open());
    std::string fragment_source((std::istreambuf_iterator<char>(fragment_file)),
                                std::istreambuf_iterator<char>());

    vertex_file.close();
    fragment_file.close();

    return Shader::from_string(vertex_source, fragment_source);
}

void Shader::bind() const {
    glUseProgram(ID);
}

void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::set_uniform_int(int value, const std::string& name) {
    bind();
    if (!m_uniform_location.contains(name)) {
        int uniform_location = glGetUniformLocation(ID, name.c_str());
        m_uniform_location.insert({name, uniform_location});
    }

    int uniform_location = m_uniform_location[name];
    glUniform1i(uniform_location, value);
}

void Shader::set_uniform_vec3(const glm::vec3& value, const std::string& name) {
    bind();
    if (!m_uniform_location.contains(name)) {
        int uniform_location = glGetUniformLocation(ID, name.c_str());
        m_uniform_location.insert({name, uniform_location});
    }

    int uniform_location = m_uniform_location[name];
    glUniform3f(uniform_location, value.x, value.y, value.z);
}

void Shader::set_uniform_mat4(const glm::mat4& value, const std::string& name) {
    bind();
    if (!m_uniform_location.contains(name)) {
        int uniform_location = glGetUniformLocation(ID, name.c_str());
        m_uniform_location.insert({name, uniform_location});
    }

    int uniform_location = m_uniform_location[name];
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, &value[0][0]);
}

Shader::Shader(unsigned int id) : ID(id) {
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

unsigned int Shader::compile(const std::string& source, unsigned int type) {
    unsigned int shader = glCreateShader(type);

    const char* shader_source = source.c_str();
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);
    return shader;
}

} // namespace renderer