#pragma once

#include "core.h"

#include "framebuffer.h"

namespace Hydrogen {

// Forward declaration
class Shader;

class HG_API Cubemap : public IFramebufferAttachable {
  public:
    struct Components {
        std::string right;
        std::string left;
        std::string top;
        std::string bottom;
        std::string front;
        std::string back;
    };

    Cubemap(bool is_mipmap = false);
    Cubemap(u32 width, u32 height, bool is_mipmap = false);
    Cubemap(const Components& faces, bool flip = false);
    Cubemap(const std::string& equirectangular_image_path, bool flip = false);
    ~Cubemap();

    void set_current_framebuffer_attached_face(u32 face);
    void attach_to_framebuffer(Framebuffer::AttachmentType attachment_type,
                               u32 level) const override;

    void bind(const std::string& name, Shader* shader, u32 slot) const;
    void unbind() const;

  private:
    u32 ID;
    u32 m_current_framebuffer_face = 0;

    void load_face_path(const std::string& path, u32 face) const;
};

} // namespace Hydrogen
