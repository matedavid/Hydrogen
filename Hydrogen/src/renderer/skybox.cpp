#include "skybox.h"

#include <filesystem>

#include "stb_image.h"
#include <glad/glad.h>

namespace Hydrogen {

// const std::string = SKYBOX_VERTEX_PATH "base.skybox.vert";
const std::string SKYBOX_VERTEX_PATH = "../../Hydrogen/assets/shaders/base.skybox.vert";
// const std::string SKYBOX_FRAGMENT_PATH = "base.skybox.frag";
const std::string SKYBOX_FRAGMENT_PATH = "../../Hydrogen/assets/shaders/base.skybox.frag";

Skybox::Skybox(const Components& components) {
    const auto faces = std::array<std::string, 6>({
        components.right,
        components.left,
        components.top,
        components.bottom,
        components.front,
        components.back
    });

    init(faces);
}

Skybox::Skybox(const Components& components, const std::string& directory) {
    const auto directory_path = std::filesystem::path(directory);

    const auto faces = std::array<std::string, 6>({
        directory_path / components.right,
        directory_path / components.left,
        directory_path / components.top,
        directory_path / components.bottom,
        directory_path / components.front,
        directory_path / components.back
    });

    init(faces);
}

void Skybox::init(const std::array<std::string, 6>& faces) {
    // Create Skybox
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    // Load face textures
    stbi_set_flip_vertically_on_load(false);

    for (u32 i = 0; i < faces.size(); ++i) {
        const auto& face_path = faces[i];

        i32 width, height, num_channels;
        unsigned char* data = stbi_load(face_path.c_str(), &width, &height, &num_channels, 0);
        if (!data) {
            HG_LOG_ERROR("Failed to load skybox texture {}", face_path);
            return;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

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

void Skybox::bind_to_shader(Shader* shader, u32 slot) const {
    shader->set_uniform_int("Skybox", (i32)slot);

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
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