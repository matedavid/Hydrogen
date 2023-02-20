#pragma once

#include "core.h"

namespace Hydrogen {

// Forward declarations
class Texture;
class Renderbuffer;
class IFramebufferAttachable;

class HG_API Framebuffer {
  public:
    Framebuffer();
    ~Framebuffer();

    void bind() const;
    void unbind() const;

    enum class AttachmentType {
        Color0,
        Depth,
        Stencil,
        DepthStencil
    };
    static u32 get_attachment_type(AttachmentType type);

    void attach(
        const IFramebufferAttachable& attachable, AttachmentType attachment_type, u32 level = 0) const;

  private:
    u32 ID;
};

class IFramebufferAttachable {
  public:
    virtual ~IFramebufferAttachable() = default;

    virtual void attach_to_framebuffer(Framebuffer::AttachmentType attachment_type, [[maybe_unused]] u32 level) const = 0;
};

} // namespace Hydrogen
