#pragma once

#include "core.h"

#include <unordered_map>
#include <functional>

#include "core/material.h"
#include "renderer/shader.h"

namespace Hydrogen {

class ShaderSystem {
  public:
    static ShaderSystem* instance;

    static void init();
    static void free();

    Shader* get(ShaderId id);

        // Acquire functions
    ShaderId acquire_from_source(const std::string& vertex_src, const std::string& fragment_src);
    ShaderId acquire_from_file(const std::string& vertex_path, const std::string& fragment_path);
    ShaderId acquire_from_material(const MaterialValues& material);

    void release(ShaderId id);

  private:
    std::unordered_map<ShaderId, Shader*> m_shaders;
    std::unordered_map<ShaderId, i32> m_reference_count;

    ShaderSystem();
    ~ShaderSystem();

    inline ShaderId hash_combine(ShaderId first, ShaderId second) {
        return second + 0x9e3779b9 + (first << 6) + (first >> 2);
    }
};

} // namespace Hydrogen
