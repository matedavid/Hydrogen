#pragma once

#include "core.h"

#include <unordered_map>
#include <functional>

#include "material.h"
#include "renderer/shader.h"

namespace Hydrogen {

using ShaderId = std::size_t;

class ShaderSystem {
  public:
    static ShaderSystem* instance;

    static void init();
    static void destroy();

    // Acquire functions
    ShaderId acquire_from_source(const std::string& vertex_src, const std::string& fragment_src);
    ShaderId acquire_from_file(const std::string& vertex_path, const std::string& fragment_path);
    ShaderId acquire_from_material(const MaterialValues& material);

    void release(int id);

  private:
    std::unordered_map<ShaderId, Shader*> m_shaders;

    ShaderSystem();
    ~ShaderSystem();
};

} // namespace Hydrogen
