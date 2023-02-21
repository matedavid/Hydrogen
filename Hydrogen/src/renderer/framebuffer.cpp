#include "framebuffer.h"

#include <glad/glad.h>

namespace Hydrogen {

Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &ID);
    bind();
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &ID);
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attach(const IFramebufferAttachable& attachable,
                         AttachmentType attachment_type,
                         u32 level) const {
    bind();
    attachable.attach_to_framebuffer(attachment_type, level);
}

u32 Framebuffer::get_attachment_type(AttachmentType type) {
    switch (type) {
        case AttachmentType::Color0:
            return GL_COLOR_ATTACHMENT0;
        case AttachmentType::Depth:
            return GL_DEPTH_ATTACHMENT;
        case AttachmentType::Stencil:
            return GL_STENCIL_ATTACHMENT;
        case AttachmentType::DepthStencil:
            return GL_DEPTH_STENCIL_ATTACHMENT;
    }

    HG_ASSERT(false, "Unreachable");
    return 0;
}

} // namespace Hydrogen
