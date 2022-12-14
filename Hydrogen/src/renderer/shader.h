#pragma once

#include "core.h"

#include <iostream>
#include <map>
#include "glm/glm.hpp"

namespace Hydrogen {

class HG_API Shader {
  public:
    static Shader* from_string(const std::string& vertex_src, const std::string& fragment_src);
    static Shader* from_file(const std::string& vertex_path, const std::string& fragment_path);

    ~Shader();

    void bind() const;
    void unbind() const;

    void set_uniform_int(int value, const std::string& name);
    void set_uniform_vec3(const glm::vec3& value, const std::string& name);
    void set_uniform_mat4(const glm::mat4& value, const std::string& name);

  private:
    unsigned int ID;

    std::map<std::string, int> m_uniform_location;

    Shader(unsigned int id);
    static unsigned int compile(const std::string& source, unsigned int type);
};

} // namespace renderer
