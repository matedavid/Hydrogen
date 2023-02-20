#pragma once

#include "core.h"

#include "renderer/framebuffer.h"

namespace Hydrogen {

class HG_API Renderbuffer : public IFramebufferAttachable {
  public:
    enum class InternalFormat {
        DepthComponent16,
        DepthComponent24,
        DepthComponent32,
    };

    Renderbuffer(u32 width, u32 height, InternalFormat internal_format);
    ~Renderbuffer();

    void set_storage(u32 width, u32 height, InternalFormat internal_format) const;

    void attach_to_framebuffer(Framebuffer::AttachmentType attachment_type, u32 level) const override;

    void bind() const;
    void unbind() const;

  private:
    u32 ID;

    u32 get_internal_format(InternalFormat internal_format) const;
};

} // namespace Hydrogen
