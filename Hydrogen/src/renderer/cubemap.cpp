#include "cubemap.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

#include "core/application.h"

#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/framebuffer.h"
#include "renderer/renderbuffer.h"
#include "renderer/renderer3d.h"
#include "renderer/renderer_api.h"

#include "systems/shader_system.h"

namespace Hydrogen {

Cubemap::Cubemap(bool is_mipmap) {
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    i32 min_filter = is_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Cubemap::Cubemap(u32 width, u32 height, bool is_mipmap) : Cubemap(is_mipmap) {
    for (u32 i = 0; i < 6; ++i) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, (i32)width, (i32)height, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    if (is_mipmap) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
}

Cubemap::Cubemap(const f32* data, u32 width, u32 height) : Cubemap(false) {
    for (u32 i = 0; i < 6; ++i) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, (i32)width, (i32)height, 0, GL_RGB, GL_FLOAT, data);
    }
}

Cubemap::Cubemap(const Components& faces, bool flip) : Cubemap() {
    stbi_set_flip_vertically_on_load(flip);

    load_face_path(faces.right, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    load_face_path(faces.left, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    load_face_path(faces.top, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    load_face_path(faces.bottom, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    load_face_path(faces.front, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    load_face_path(faces.back, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Cubemap::Cubemap(const std::string& equirectangular_image_path, bool flip) : Cubemap() {
    // Load image
    stbi_set_flip_vertically_on_load(flip);

    i32 width, height, components;
    f32* data = stbi_loadf(equirectangular_image_path.c_str(), &width, &height, &components, 0);
    if (!data) {
        HG_LOG_ERROR("Could not load image file: {}", equirectangular_image_path);
        return;
    }

    // set up projection and view matrices for capturing data onto the 6 cubemap face directions
    const auto capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    const auto capture_views = std::vector<glm::mat4>{
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    // Size of cubemap faces
    const i32 SIZE = 512;

    // Create image texture
    const auto texture = Texture(data, width, height);
    stbi_image_free(data);

    // Convert equirectangular image to cubemap
    const auto framebuffer = Framebuffer();
    const auto renderbuffer =
        Renderbuffer(SIZE, SIZE, Renderbuffer::InternalFormat::DepthComponent24);
    framebuffer.attach(renderbuffer, Framebuffer::AttachmentType::Depth);

    // Set width and height for cubemap faces
    for (u32 i = 0; i < 6; ++i) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, SIZE, SIZE, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    usize equirectangular_cubemap_id =
        ShaderSystem::instance->acquire_base("base.skybox_operations.vert", "base.equirectangular.frag");
    auto* equirectangular_cubemap_shader = ShaderSystem::instance->get(equirectangular_cubemap_id);

    equirectangular_cubemap_shader->set_uniform_mat4("Projection", capture_projection);
    texture.bind("EquirectangularMap", equirectangular_cubemap_shader, 0);

    RendererAPI::resize(SIZE, SIZE);
    framebuffer.bind();

    for (u32 i = 0; i < 6; ++i) {
        equirectangular_cubemap_shader->set_uniform_mat4("View", capture_views[i]);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ID, 0);

        // TODO: Should have specific functions to clear color and buffer bits
        RendererAPI::clear(glm::vec3(0.0f));

        Renderer3D::draw_cube(glm::vec3(0.0f), glm::vec3(2.0f), equirectangular_cubemap_shader);
    }

    // Return to default framebuffer and to original size
    framebuffer.unbind();
    i32 original_width = Application::get()->get_window()->get_width();
    i32 original_height = Application::get()->get_window()->get_height();
    RendererAPI::resize(original_width, original_height);

    // Release shader
    ShaderSystem::instance->release(equirectangular_cubemap_id);
}

Cubemap::~Cubemap() {
    glDeleteTextures(1, &ID);
}

void Cubemap::set_current_framebuffer_attached_face(u32 face) {
    m_current_framebuffer_face = face;
}

void Cubemap::attach_to_framebuffer(Framebuffer::AttachmentType attachment_type, u32 level) const {
    u32 attachment = Framebuffer::get_attachment_type(attachment_type);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, m_current_framebuffer_face, ID, (i32)level);
}

void Cubemap::bind(const std::string& name, Shader* shader, u32 slot) const {
    shader->set_uniform_int(name, (i32)slot);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void Cubemap::unbind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Cubemap::load_face_path(const std::string& path, u32 face) const {
    auto extension = std::filesystem::path(path).extension();

    i32 width, height, num_channels;
    if (extension == ".hdr" || extension == ".hdri") {
        auto* data = stbi_loadf(path.c_str(), &width, &height, &num_channels, 4);
        if (!data) {
            HG_LOG_ERROR("Failed to load skybox texture {}", path);
            return;
        }

        glTexImage2D(face, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
        stbi_image_free(data);
    } else {
        auto* data = stbi_load(path.c_str(), &width, &height, &num_channels, 0);
        if (!data) {
            HG_LOG_ERROR("Failed to load skybox texture {}", path);
            return;
        }

        glTexImage2D(face, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
}

} // namespace Hydrogen
