#include "shader.h"

#include <glad/glad.h>

#include <fstream>
#include <vector>

namespace Hydrogen {

Shader* Shader::from_string(const std::string& vertex_src, const std::string& fragment_src) {
    // Compile Shaders
    u32 vertex_shader = Shader::compile(vertex_src, GL_VERTEX_SHADER);
    u32 fragment_shader = Shader::compile(fragment_src, GL_FRAGMENT_SHADER);

    // Shader program
    u32 shader_program = glCreateProgram();
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
    HG_ASSERT(vertex_file.is_open(), "Could not open vertex shader file: {}", vertex_path);
    std::string vertex_source((std::istreambuf_iterator<char>(vertex_file)),
                              std::istreambuf_iterator<char>());

    std::ifstream fragment_file(fragment_path);
    HG_ASSERT(fragment_file.is_open(), "Could not open fragment shader file: {}", fragment_path);
    std::string fragment_source((std::istreambuf_iterator<char>(fragment_file)),
                                std::istreambuf_iterator<char>());

    vertex_file.close();
    fragment_file.close();

    return Shader::from_string(vertex_source, fragment_source);
}

Shader* Shader::default_() {
    const std::string& vertex_src = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 v_TexCoord;

layout(std140) uniform Camera {
    uniform mat4 Projection;
    uniform mat4 View;
    uniform vec3 CameraPosition;
};

uniform mat4 Model;

void main()
{
    gl_Position = Projection * View * Model * vec4(aPos, 1.0f);
    v_TexCoord = aTexCoord;
}
)";

    const std::string& fragment_src = R"(
#version 330 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform vec3 Color;
uniform sampler2D Texture;

void main()
{
    FragColor = texture(Texture, v_TexCoord) * vec4(Color, 1.0f);
}
)";

    return Shader::from_string(vertex_src, fragment_src);
}

void Shader::bind() const {
    glUseProgram(ID);
}

void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::assign_uniform_buffer(const std::string& name, UniformBuffer* uniform_buffer, u32 slot) const {
    uniform_buffer->assign_slot(slot);

    u32 uniform_block = glGetUniformBlockIndex(ID, name.c_str());
    glUniformBlockBinding(ID, uniform_block, slot);
}

void Shader::set_uniform_int(const std::string& name, i32 value) {
    bind();
    if (!m_uniform_location.contains(name)) {
        i32 uniform_location = get_uniform_location(name);
        m_uniform_location.insert({name, uniform_location});
    }

    i32 uniform_location = m_uniform_location[name];
    glUniform1i(uniform_location, value);
}

void Shader::set_uniform_float(const std::string& name, f32 value) {
    bind();
    if (!m_uniform_location.contains(name)) {
        i32 uniform_location = get_uniform_location(name);
        m_uniform_location.insert({name, uniform_location});
    }

    i32 uniform_location = m_uniform_location[name];
    glUniform1f(uniform_location, value);
}

void Shader::set_uniform_vec3(const std::string& name, const glm::vec3& value) {
    bind();
    if (!m_uniform_location.contains(name)) {
        i32 uniform_location = get_uniform_location(name);
        m_uniform_location.insert({name, uniform_location});
    }

    i32 uniform_location = m_uniform_location[name];
    glUniform3f(uniform_location, value.x, value.y, value.z);
}

void Shader::set_uniform_mat4(const std::string& name, const glm::mat4& value) {
    bind();
    if (!m_uniform_location.contains(name)) {
        i32 uniform_location = get_uniform_location(name);
        m_uniform_location.insert({name, uniform_location});
    }

    i32 uniform_location = m_uniform_location[name];
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, &value[0][0]);
}

Shader::Shader(u32 id) : ID(id) {
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

u32 Shader::compile(const std::string& source, u32 type) {
    u32 shader = glCreateShader(type);

    const char* shader_source = source.c_str();
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (compiled == GL_FALSE) {
        GLint max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        // The maxLength includes the NULL character
        std::vector<GLchar> error_log((usize)max_length);
        glGetShaderInfoLog(shader, max_length, &max_length, &error_log[0]);

        std::string str_type = (type == GL_VERTEX_SHADER) ? "Vertex: " : "Fragment: ";

        std::string message(error_log.begin(), error_log.end());
        throw std::runtime_error(str_type + message);
    }

    return shader;
}

i32 Shader::get_uniform_location(const std::string& name) {
    i32 uniform_location = glGetUniformLocation(ID, name.c_str());
    if (uniform_location == -1) {
        HG_LOG_WARN("Uniform '{}' not found in shader with ID {}", name, ID);
    }

    return uniform_location;
}

} // namespace renderer