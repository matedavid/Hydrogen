#include "skybox.h"

#include <filesystem>
#include <cmath>

#include "stb_image.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/renderer3d.h"
#include "core/application.h"
#include "renderer/framebuffer.h"
#include "renderer/renderbuffer.h"
#include "renderer/renderer_api.h"

namespace Hydrogen {

Skybox::Skybox(const Cubemap::Components& faces) {
    m_cubemap = new Cubemap(faces);

    create_diffuse_irradiance_map();
    create_specular_radiance_map();

    // Get Skybox shader
    m_shader_id = ShaderSystem::instance->acquire_base("base.skybox.vert", "base.skybox.frag");
}

Skybox::Skybox(const Cubemap::Components& faces, const std::string& directory) {
    const auto directory_path = std::filesystem::path(directory);

    const auto directory_faces = Cubemap::Components{
        directory_path / faces.right,
        directory_path / faces.left,
        directory_path / faces.top,
        directory_path / faces.bottom,
        directory_path / faces.front,
        directory_path / faces.back
    };

    m_cubemap = new Cubemap(directory_faces);

    // Get Skybox shader
    m_shader_id = ShaderSystem::instance->acquire_base("base.skybox.vert", "base.skybox.frag");

    create_diffuse_irradiance_map();
    create_specular_radiance_map();
}

Skybox::Skybox(const std::string& image_path) {
    // Check if its hdr
    const auto extension = std::filesystem::path(image_path).extension();
    m_is_hdr = extension == ".hdr" || extension == ".hdri";

    // Get cubemap from image path
    m_cubemap = new Cubemap(image_path, true); // TODO: Flip should be configurable

    // Get Skybox shader
    m_shader_id = ShaderSystem::instance->acquire_base("base.skybox.vert", "base.skybox.frag");

    create_diffuse_irradiance_map();
    create_specular_radiance_map();
}

Skybox::~Skybox() {
    delete m_cubemap;
    delete m_irradiance_map;
    delete m_prefilter;
    delete m_brdf_texture;
}

Shader* Skybox::bind(u32 slot) const {
    auto* shader = ShaderSystem::instance->get(m_shader_id);
    m_cubemap->bind("Skybox", shader, slot);

    i32 post_processing_value = m_is_hdr ? 1 : 0;
    shader->set_uniform_int("PostProcessing", post_processing_value);

    return shader;
}

void Skybox::bind_to_shader(Shader* shader, u32 slot) const {
    m_irradiance_map->bind("IrradianceMap", shader, slot);
    m_prefilter->bind("PrefilterMap", shader, slot + 1);
    m_brdf_texture->bind("BrdfLUT", shader, slot + 2);
}

void Skybox::unbind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::create_diffuse_irradiance_map() {
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

    const auto framebuffer = Framebuffer();
    const auto renderbuffer = Renderbuffer(32, 32, Renderbuffer::InternalFormat::DepthComponent24);

    // Create irradiance map cubemap
    m_irradiance_map = new Cubemap(32, 32);

    usize convolution_shader_id =
        ShaderSystem::instance->acquire_base("base.equirectangular.vert", "base.convolution.frag");
    auto* convolution_shader = ShaderSystem::instance->get(convolution_shader_id);

    convolution_shader->set_uniform_mat4("Projection", capture_projection);
    m_cubemap->bind("Skybox", convolution_shader, 0);

    RendererAPI::resize(32, 32);
    framebuffer.bind();

    for (u32 i = 0; i < 6; ++i) {
        convolution_shader->set_uniform_mat4("View", capture_views[i]);

        m_irradiance_map->set_current_framebuffer_attached_face(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
        framebuffer.attach(*m_irradiance_map, Framebuffer::AttachmentType::Color0);

        RendererAPI::clear(glm::vec3(0.0f));

        Renderer3D::draw_cube(glm::vec3(0.0f), glm::vec3(2.0f), convolution_shader);
    }

    framebuffer.unbind();

    // Cleanup
    ShaderSystem::instance->release(convolution_shader_id);

    i32 original_width = Application::get()->get_window()->get_width();
    i32 original_height = Application::get()->get_window()->get_height();
    RendererAPI::resize(original_width, original_height);
}

static void renderQuad();

void Skybox::create_specular_radiance_map() {
    // set up projection and view matrices for capturing data onto the 6 cubemap face directions
    const auto capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    const auto capture_views = std::vector<glm::mat4>{
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    const auto framebuffer = Framebuffer();
    const auto renderbuffer =
        Renderbuffer(512, 512, Renderbuffer::InternalFormat::DepthComponent24);
    framebuffer.attach(renderbuffer, Framebuffer::AttachmentType::Depth);

    //
    // Prefilter
    //
    m_prefilter = new Cubemap(128, 128, true);
    m_prefilter->unbind();

    // Get shaders
    usize prefilter_shader_id =
        ShaderSystem::instance->acquire_base("base.equirectangular.vert", "base.prefilter.frag");
    usize brdf_shader_id =
        ShaderSystem::instance->acquire_base("base.brdf.vert", "base.brdf.frag");

    auto* prefilter_shader = ShaderSystem::instance->get(prefilter_shader_id);
    auto* brdf_shader = ShaderSystem::instance->get(brdf_shader_id);

    prefilter_shader->set_uniform_mat4("Projection", capture_projection);
    m_cubemap->bind("EnvironmentMap", prefilter_shader, 0);

    framebuffer.bind();

    const u32 MAX_MIP_LEVELS = 5;
    for (u32 mip = 0; mip < MAX_MIP_LEVELS; ++mip) {
        // resize framebuffer according to mip-level size.
        u32 mip_width  = (u32)(128u * std::pow(0.5f, mip));
        u32 mip_height = (u32)(128u * std::pow(0.5f, mip));

        renderbuffer.bind();
        renderbuffer.set_storage(mip_width, mip_height,
                                 Renderbuffer::InternalFormat::DepthComponent24);

        RendererAPI::resize((i32)mip_width, (i32)mip_height);

        f32 roughness = (f32)mip / (f32)(MAX_MIP_LEVELS - 1);
        prefilter_shader->set_uniform_float("roughness", roughness);

        for (u32 i = 0; i < 6; ++i) {
            prefilter_shader->set_uniform_mat4("View", capture_views[i]);

            m_prefilter->set_current_framebuffer_attached_face(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
            framebuffer.attach(*m_prefilter, Framebuffer::AttachmentType::Color0, mip);

            RendererAPI::clear(glm::vec3(0.0f));

            Renderer3D::draw_cube(glm::vec3(0.0f), glm::vec3(2.0f), prefilter_shader);
        }
    }

    framebuffer.unbind();

    //
    // Precompute BRDF
    //
    m_brdf_texture = new Texture((const f32*)nullptr, 512, 512);

    framebuffer.bind();
    renderbuffer.bind();
    renderbuffer.set_storage(512, 512, Renderbuffer::InternalFormat::DepthComponent24);

    framebuffer.attach(*m_brdf_texture, Framebuffer::AttachmentType::Color0);

    RendererAPI::resize(512, 512);
    brdf_shader->bind();
    RendererAPI::clear(glm::vec3(0.0f));

    renderQuad();

    framebuffer.unbind();

    // Cleanup
    ShaderSystem::instance->release(prefilter_shader_id);
    ShaderSystem::instance->release(brdf_shader_id);

    i32 original_width = Application::get()->get_window()->get_width();
    i32 original_height = Application::get()->get_window()->get_height();
    RendererAPI::resize(original_width, original_height);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
static void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        unsigned int quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

} // namespace Hydrogen