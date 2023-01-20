#pragma once

#include "core.h"

#include <glm/glm.hpp>

#include <iostream>
#include <map>

#include "buffers.h"

namespace Hydrogen {

class HG_API Shader {
  public:
    static Shader* from_string(const std::string& vertex_src, const std::string& fragment_src);
    static Shader* from_file(const std::string& vertex_path, const std::string& fragment_path);
    static Shader* default_();

    ~Shader();

    void bind() const;
    void unbind() const;

    void assign_uniform_buffer(const std::string& name, const UniformBuffer* uniform_buffer, unsigned int slot) const;

    void set_uniform_int(const std::string& name, int value);
    void set_uniform_float(const std::string& name, float value);
    void set_uniform_vec3(const std::string& name, const glm::vec3& value);
    void set_uniform_mat4(const std::string& name, const glm::mat4& value);

  private:
    unsigned int ID;

    std::map<std::string, int> m_uniform_location;

    Shader(unsigned int id);
    static unsigned int compile(const std::string& source, unsigned int type);
};

} // namespace renderer
