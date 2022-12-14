#pragma once

#include "core.h"

#include <string>

namespace Hydrogen {

class HG_API Texture {
  public:
    Texture(const unsigned char* data, int width, int height);
    Texture(const std::string& path);
    ~Texture();

    static Texture* white();

    int get_width() const { return m_width; }
    int get_height() const { return m_height; }

    void bind(unsigned int slot = 0) const;
    void unbind() const;

  private:
    unsigned int ID;

    std::string m_file_path;
    int m_width, m_height, m_BPP;
};

} // namespace Hydrogen
