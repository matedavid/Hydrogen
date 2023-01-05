#pragma once

#include "core.h"

#include <string>
#include <unordered_map>

#include "renderer/texture.h"

namespace Hydrogen {

#define DEFAULT_TEXTURE_NAME "default"

class TextureSystem {
  public:
    ~TextureSystem();
    static TextureSystem* get();

    Texture* acquire(const std::string& texture_path);
    void release(const Texture* texture);

    Texture* default_texture() const;

  private:
    std::unordered_map<std::string, Texture*> m_textures;
    std::unordered_map<std::string, int> m_reference_count;

    static TextureSystem* m_instance;
    TextureSystem();
};

} // namespace Hydrogen
