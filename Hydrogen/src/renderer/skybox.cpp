#include "skybox.h"

#include <filesystem>
#include <cmath>

#include "stb_image.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer3d.h"
#include "core/application.h"

namespace Hydrogen {

// const std::string SKYBOX_VERTEX_PATH = "base.skybox.vert";
const std::string SKYBOX_VERTEX_PATH = "../../Hydrogen/assets/shaders/base.skybox.vert";
// const std::string SKYBOX_FRAGMENT_PATH = "base.skybox.frag";
const std::string SKYBOX_FRAGMENT_PATH = "../../Hydrogen/assets/shaders/base.skybox.frag";

// const std::string EQUIRECTANGULAR_VERTEX_PATH = "base.equirectangular.vert";
const std::string EQUIRECTANGULAR_VERTEX_PATH = "../../Hydrogen/assets/shaders/base.equirectangular.vert";
// const std::string EQUIRECTANGULAR_FRAGMENT_PATH = "base.equirectangular.frag";
const std::string EQUIRECTANGULAR_FRAGMENT_PATH = "../../Hydrogen/assets/shaders/base.equirectangular.frag";

// const std::string CONVOLUTION_VERTEX_PATH = "base.equirectangular.vert";
const std::string CONVOLUTION_VERTEX_PATH = "../../Hydrogen/assets/shaders/base.equirectangular.vert";
// const std::string CONVOLUTION_FRAGMENT_PATH = "base.convolution.frag";
const std::string CONVOLUTION_FRAGMENT_PATH = "../../Hydrogen/assets/shaders/base.convolution.frag";

Skybox::Skybox(const Components& components) {
    const auto faces = std::array<std::string, 6>({
        components.right,
        components.left,
        components.top,
        components.bottom,
        components.front,
        components.back
    });

    init(faces);
}

Skybox::Skybox(const Components& components, const std::string& directory) {
    const auto directory_path = std::filesystem::path(directory);

    const auto faces = std::array<std::string, 6>({
        directory_path / components.right,
        directory_path / components.left,
        directory_path / components.top,
        directory_path / components.bottom,
        directory_path / components.front,
        directory_path / components.back
    });

    init(faces);
}

Skybox::Skybox(const std::string& image_path) {
    // Check if its hdr
    const auto extension = std::filesystem::path(image_path).extension();
    m_is_hdr = extension == ".hdr" || extension == ".hdri";

    // Load image
    stbi_set_flip_vertically_on_load(true);

    i32 width, height, components;
    f32* data = stbi_loadf(image_path.c_str(), &width, &height, &components, 0);
    if (!data) {
        HG_LOG_ERROR("Could not load image file: {}", image_path);
        return;
    }

    // Create image texture
    const auto texture = Texture(data, width, height);
    stbi_image_free(data);

    // Create framebuffers
    u32 capture_fbo, capture_rbo;
    glGenFramebuffers(1, &capture_fbo);
    glGenRenderbuffers(1, &capture_rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, capture_rbo);

    // Setup cubemap
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    for (u32 i = 0; i < 6; ++i) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // set up projection and view matrices for capturing data onto the 6 cubemap face directions
    const auto captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    const auto captureViews = std::vector<glm::mat4>{
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // Create conversion shader
    auto equirectangular_cubemap_shader =
        Shader::from_file(EQUIRECTANGULAR_VERTEX_PATH, EQUIRECTANGULAR_FRAGMENT_PATH);

    // convert equirectangular environment map to cubemap
    equirectangular_cubemap_shader->set_uniform_int("EquirectangularMap", 0);
    equirectangular_cubemap_shader->set_uniform_mat4("Projection", captureProjection);
    texture.bind(0);

    // configure the viewport to the capture dimensions.
    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangular_cubemap_shader->set_uniform_mat4("View", captureViews[i]);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Renderer3D::draw_cube(
            glm::vec3(0.0f), glm::vec3(2.0f), equirectangular_cubemap_shader);
    }

    // Set default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Release framebuffers and other stuff
    glDeleteFramebuffers(1, &capture_fbo);
    glDeleteRenderbuffers(1, &capture_rbo);
    delete equirectangular_cubemap_shader;

    // configure the viewport to the original framebuffer's screen dimensions
    i32 original_width = Application::get()->get_window()->get_width();
    i32 original_height = Application::get()->get_window()->get_height();
    glViewport(0, 0, original_width, original_height);

    // Get Skybox shader
    m_shader_id =
        ShaderSystem::instance->acquire_from_file(SKYBOX_VERTEX_PATH, SKYBOX_FRAGMENT_PATH);

    create_diffuse_irradiance_map();
    create_specular_radiance_map();
}

void Skybox::init(const std::array<std::string, 6>& faces) {
    // Create Skybox
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    // Load face textures
    stbi_set_flip_vertically_on_load(false);

    for (u32 i = 0; i < faces.size(); ++i) {
        const auto& face_path = faces[i];

        // Check if is HDR (if one image is HDR, all images will be treated as HDR)
        const auto extension = std::filesystem::path(face_path).extension();
        m_is_hdr = m_is_hdr || extension == ".hdr" || extension == ".hdri";

        i32 width, height, num_channels;
        unsigned char* data = stbi_load(face_path.c_str(), &width, &height, &num_channels, 0);
        if (!data) {
            HG_LOG_ERROR("Failed to load skybox texture {}", face_path);
            return;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    // Configuration
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    unbind();

    // Get Skybox shader
    m_shader_id =
        ShaderSystem::instance->acquire_from_file(SKYBOX_VERTEX_PATH, SKYBOX_FRAGMENT_PATH);

    create_diffuse_irradiance_map();
    create_specular_radiance_map();
}

Skybox::~Skybox() {
    glDeleteTextures(1, &ID);
    glDeleteTextures(1, &IrradianceID);
    glDeleteTextures(1, &PrefilterID);
    glDeleteTextures(1, &BrdfID);
}

Shader* Skybox::bind(u32 slot) const {
    auto* shader = ShaderSystem::instance->get(m_shader_id);
    shader->set_uniform_int("Skybox", (i32)slot);

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    i32 post_processing_value = m_is_hdr ? 1 : 0;
    shader->set_uniform_int("PostProcessing", post_processing_value);

    return shader;
}

void Skybox::bind_to_shader(Shader* shader, u32 slot) const {
    shader->set_uniform_int("IrradianceMap", (i32)slot);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceID);

    shader->set_uniform_int("PrefilterMap", (i32)slot + 1);
    glActiveTexture(GL_TEXTURE0 + slot + 1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, PrefilterID);

    shader->set_uniform_int("BrdfLUT", (i32)slot + 2);
    glActiveTexture(GL_TEXTURE0 + slot + 2);
    glBindTexture(GL_TEXTURE_2D, BrdfID);
}

void Skybox::unbind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::create_diffuse_irradiance_map() {
    // Create framebuffers
    u32 capture_fbo, capture_rbo;
    glGenFramebuffers(1, &capture_fbo);
    glGenRenderbuffers(1, &capture_rbo);

    // Create irradiance cubemap
    glGenTextures(1, &IrradianceID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceID);
    for (u32 i = 0; i < 6; ++i) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

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

    auto* convolution_shader = Shader::from_file(CONVOLUTION_VERTEX_PATH, CONVOLUTION_FRAGMENT_PATH);

    convolution_shader->set_uniform_int("Skybox", 0);
    convolution_shader->set_uniform_mat4("Projection", capture_projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    // Configure viewport to capture directions
    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
    for (u32 i = 0; i < 6; ++i) {
        convolution_shader->set_uniform_mat4("View", capture_views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IrradianceID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Renderer3D::draw_cube(
            glm::vec3(0.0f), glm::vec3(2.0f), convolution_shader);
    }

    // Bind default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Release framebuffers and other stuff
    glDeleteFramebuffers(1, &capture_fbo);
    glDeleteRenderbuffers(1, &capture_rbo);
    delete convolution_shader;
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // configure the viewport to the original framebuffer's screen dimensions
    i32 original_width = Application::get()->get_window()->get_width();
    i32 original_height = Application::get()->get_window()->get_height();
    glViewport(0, 0, original_width, original_height);
}

static void renderQuad();

void Skybox::create_specular_radiance_map() {
    // Generate prefilter texture
    glGenTextures(1, &PrefilterID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, PrefilterID);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Notice GL_LINEAR_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Reserve enough space in the GPU for MIPMAPs
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // Create framebuffers
    u32 capture_fbo, capture_rbo;
    glGenFramebuffers(1, &capture_fbo);
    glGenRenderbuffers(1, &capture_rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, capture_rbo);

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

    auto* prefilter_shader = Shader::from_file(CONVOLUTION_VERTEX_PATH, "../../Hydrogen/assets/shaders/base.prefilter.frag");
    prefilter_shader->set_uniform_int("EnvironmentMap", 0);
    prefilter_shader->set_uniform_mat4("Projection", capture_projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        // resize framebuffer according to mip-level size.
        unsigned int mipWidth  = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, (i32)mipWidth, (i32)mipHeight);
        glViewport(0, 0, (i32)mipWidth, (i32)mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilter_shader->set_uniform_float("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i) {
            prefilter_shader->set_uniform_mat4("View", capture_views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, PrefilterID, (i32)mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Renderer3D::draw_cube(glm::vec3(0.0f), glm::vec3(2.0f), prefilter_shader);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //
    // Precompute BRDF
    //
    glGenTextures(1, &BrdfID);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, BrdfID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto brdf_shader = Shader::from_file("../../Hydrogen/assets/shaders/base.brdf.vert", "../../Hydrogen/assets/shaders/base.brdf.frag");

    glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BrdfID, 0);

    glViewport(0, 0, 512, 512);
    brdf_shader->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Release framebuffers and other stuff
    glDeleteFramebuffers(1, &capture_fbo);
    glDeleteRenderbuffers(1, &capture_rbo);
    delete prefilter_shader;
    delete brdf_shader;

    // configure the viewport to the original framebuffer's screen dimensions
    i32 original_width = Application::get()->get_window()->get_width();
    i32 original_height = Application::get()->get_window()->get_height();
    glViewport(0, 0, original_width, original_height);
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