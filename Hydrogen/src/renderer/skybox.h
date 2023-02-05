#pragma once

#include "core.h"

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

    Skybox(Components components);
    ~Skybox();

    void bind() const;
    void unbind() const;

  private:
    u32 ID;

    enum class SkyboxFace {
        Right = 0,
        Left = 1,
        Top = 2,
        Bottom = 3,
        Front = 4,
        Back = 5
    };

    void load_face(const std::string& path, SkyboxFace face);
};

} // namespace Hydrogen
