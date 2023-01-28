#include "texture_system.h"

namespace Hydrogen {

TextureSystem* TextureSystem::instance = nullptr;

void TextureSystem::init() {
    HG_ASSERT(instance == nullptr, "You can only initialize ShaderSystem once");
    instance = new TextureSystem();
}

void TextureSystem::free() {
    HG_ASSERT(instance != nullptr, "You must initialize ShaderSystem before it's destroyed");
    delete instance;
}

TextureSystem::TextureSystem() {
    m_textures.insert({DEFAULT_TEXTURE_NAME, Texture::white()});
}

TextureSystem::~TextureSystem() {
    for (auto value : m_textures) {
        delete value.second;
    }
}

Texture* TextureSystem::acquire(const std::string& texture_path) {
    if (texture_path == DEFAULT_TEXTURE_NAME) {
        HG_LOG_INFO("Trying to acquire default texture through TextureSystem::acquire, should use "
                    "TextureSystem::default_texture");
        return m_textures[DEFAULT_TEXTURE_NAME];
    }

    if (m_textures.contains(texture_path)) {
        m_reference_count[texture_path]++;
        return m_textures[texture_path];
    }

    HG_LOG_INFO("Loading new texture: {}", texture_path);

    auto* texture = new Texture(texture_path);
    m_textures.insert({texture_path, texture});
    m_reference_count.insert({texture_path, 1});
    return texture;
}

void TextureSystem::release(const Texture* texture) {
    if (m_textures.at(DEFAULT_TEXTURE_NAME) == texture) {
        return;
    }

    const auto& path = texture->get_path();
    if (--m_reference_count[path] == 0) {
        delete m_textures[path];
        m_textures.erase(path);
        m_reference_count.erase(path);
    }
}

Texture* TextureSystem::default_texture() const {
    return m_textures.at(DEFAULT_TEXTURE_NAME);
}

} // namespace Hydrogen
