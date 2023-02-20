#include "renderbuffer.h"

#include <glad/glad.h>
#include "renderer/framebuffer.h"

namespace Hydrogen {

Renderbuffer::Renderbuffer(u32 width, u32 height, InternalFormat internal_format) {
    glGenRenderbuffers(1, &ID);
    bind();

    u32 _internal_format = get_internal_format(internal_format);
    glRenderbufferStorage(GL_RENDERBUFFER, _internal_format, (i32)width, (i32)height);
}

Renderbuffer::~Renderbuffer() {
    glDeleteRenderbuffers(1, &ID);
}

void Renderbuffer::set_storage(u32 width, u32 height, InternalFormat internal_format) const {
    u32 _internal_format = get_internal_format(internal_format);
    glRenderbufferStorage(GL_RENDERBUFFER, _internal_format, (i32)width, (i32)height);
}

void Renderbuffer::attach_to_framebuffer(Framebuffer::AttachmentType attachment_type, [[maybe_unused]] u32 level) const {
    u32 attachment = Framebuffer::get_attachment_type(attachment_type);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, ID);
}

void Renderbuffer::bind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, ID);
}

void Renderbuffer::unbind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

u32 Renderbuffer::get_internal_format(InternalFormat internal_format) const {
    switch (internal_format) {
        case InternalFormat::DepthComponent16:
            return GL_DEPTH_COMPONENT16;
        case InternalFormat::DepthComponent24:
            return GL_DEPTH_COMPONENT24;
        case InternalFormat::DepthComponent32:
            return GL_DEPTH_COMPONENT32;
    }
    return 0;
}

} // namespace Hydrogen
