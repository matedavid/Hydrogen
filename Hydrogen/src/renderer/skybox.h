#pragma once

#include "core.h"

#include "systems/shader_system.h"

namespace Hydrogen {

class HG_API Skybox {
  public:
    struct Components {
        std::string right;
        std::string left;
        std::string top;
        std::string bottom;
        std::string front;
        std::string back;
    };

    Skybox(const Components& components);
    Skybox(const Components& components, const std::string& directory);
    ~Skybox();

    Shader* bind(u32 slot = 0) const;
    void bind_to_shader(Shader* shader, u32 slot = 0) const;
    void unbind() const;

  private:
    u32 ID;
    ShaderId m_shader_id;

    enum class SkyboxFace {
        Right = 0,
        Left = 1,
        Top = 2,
        Bottom = 3,
        Front = 4,
        Back = 5
    };

    void init(const std::array<std::string, 6>& faces);
    void load_face(const std::string& path, SkyboxFace face);
};

} // namespace Hydrogen
