#pragma once

#include "core.h"

#include <string>

namespace Hydrogen {

class HG_API Texture {
  public:
    Texture(const unsigned char* data, i32 width, i32 height);
    Texture(const f32* data, i32 width, i32 height);
    Texture(const std::string& path);
    ~Texture();

    static Texture* white();

    i32 get_width() const { return m_width; }
    i32 get_height() const { return m_height; }

    const std::string& get_path() const { return m_file_path; }

    void bind(u32 slot = 0) const;
    void unbind() const;

  private:
    u32 ID;

    std::string m_file_path;
    i32 m_width, m_height, m_BPP;
};

} // namespace Hydrogen
