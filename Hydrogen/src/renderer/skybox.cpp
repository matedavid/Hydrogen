#include "skybox.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Hydrogen {

// const std::string = SKYBOX_VERTEX_PATH "base.skybox.vert";
const std::string SKYBOX_VERTEX_PATH = "../../Hydrogen/assets/shaders/base.skybox.vert";
// const std::string SKYBOX_FRAGMENT_PATH = "base.skybox.frag";
const std::string SKYBOX_FRAGMENT_PATH = "../../Hydrogen/assets/shaders/base.skybox.frag";

Skybox::Skybox(Components components) {
    // Create Skybox
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    // Load face textures
    stbi_set_flip_vertically_on_load(false);

    load_face(components.right, SkyboxFace::Right);
    load_face(components.left, SkyboxFace::Left);
    load_face(components.top, SkyboxFace::Top);
    load_face(components.bottom, SkyboxFace::Bottom);
    load_face(components.front, SkyboxFace::Front);
    load_face(components.back, SkyboxFace::Back);

    // Configuration
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    unbind();

    // Get Skybox shader
    m_shader_id =
        ShaderSystem::instance->acquire_from_file(SKYBOX_VERTEX_PATH, SKYBOX_FRAGMENT_PATH);
}

Skybox::~Skybox() {
    glDeleteTextures(1, &ID);
}

Shader* Skybox::bind(u32 slot) const {
    auto* shader = ShaderSystem::instance->get(m_shader_id);
    shader->set_uniform_int("Skybox", (i32)slot);

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    return shader;
}

void Skybox::unbind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::load_face(const std::string& path, SkyboxFace face) {
    i32 width, height, num_channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &num_channels, 0);
    if (!data) {
        HG_LOG_ERROR("Failed to load skybox texture {}", path);
        return;
    }

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (u32)face, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}

} // namespace Hydrogen