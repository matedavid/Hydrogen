#include "texture.h"

#include <glad/glad.h>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Hydrogen {

Texture::Texture(const unsigned char* data, i32 width, i32 height)
    : m_file_path(""), m_width(width), m_height(height), m_BPP(0)
{
    glGenTextures(1, &ID);
    bind();

    // How the texture will be resampled down if it needs to be smaller than it is
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // How the texture will be resampled up if it needs to be larger than it is
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    unbind();
}

Texture::Texture(const std::string& path)
    : m_file_path(path), m_width(0), m_height(0), m_BPP(0)
{
    HG_ASSERT(std::filesystem::exists(path), "Could not open " + path);

    stbi_set_flip_vertically_on_load(true);
    unsigned char* local_buffer = stbi_load(path.c_str(), &m_width, &m_height, &m_BPP, 4);

    glGenTextures(1, &ID);
    bind();

    // How the texture will be resampled down if it needs to be smaller than it is
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // How the texture will be resampled up if it needs to be larger than it is
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, local_buffer);

    unbind();
    stbi_image_free(local_buffer);
}

Texture::~Texture() {
    glDeleteTextures(1, &ID);
}

Texture* Texture::white() {
    const unsigned char data[] = { 255, 255, 255, 255 };
    return new Texture(data, 1, 1);
}

void Texture::bind(u32 slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace Hydrogen
