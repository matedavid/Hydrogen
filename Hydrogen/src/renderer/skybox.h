#pragma once

#include "core.h"

#include "systems/shader_system.h"
#include "renderer/cubemap.h"
#include "renderer/texture.h"

namespace Hydrogen {

class HG_API Skybox {
  public:
    Skybox(const Cubemap::Components& faces);
    Skybox(const Cubemap::Components& faces, const std::string& directory);
    Skybox(const std::string& image_path);
    ~Skybox();

    Shader* bind(u32 slot = 0) const;
    void bind_to_shader(Shader* shader, u32 slot = 0) const;
    void unbind() const;

  private:
    // u32 ID;
    // u32 IrradianceID, PrefilterID, BrdfID;

    Cubemap* m_cubemap;
    Cubemap* m_irradiance_map;
    Cubemap* m_prefilter;
    Texture* m_brdf_texture;

    ShaderId m_shader_id;
    bool m_is_hdr;

    void init(const std::array<std::string, 6>& faces);
    void create_diffuse_irradiance_map();
    void create_specular_radiance_map();
};

} // namespace Hydrogen
