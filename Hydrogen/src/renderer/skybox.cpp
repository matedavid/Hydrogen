#include "skybox.h"

#include <filesystem>

#include "stb_image.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
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

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
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
}

void Skybox::init(const std::array<std::string, 6>& faces) {
    // Create Skybox
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    // Load face textures
    stbi_set_flip_vertically_on_load(false);

    for (u32 i = 0; i < faces.size(); ++i) {
        const auto& face_path = faces[i];

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
}

Skybox::~Skybox() {
    glDeleteTextures(1, &ID);
}

Shader* Skybox::bind(u32 slot) const {
    auto* shader = ShaderSystem::instance->get(m_shader_id);
    shader->set_uniform_int("Skybox", (i32)slot);

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    return shader;
}

void Skybox::bind_to_shader(Shader* shader, u32 slot) const {
    shader->set_uniform_int("Skybox", (i32)slot);

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void Skybox::unbind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::load_face(const std::string& path, SkyboxFace face) {
    i32 width, height, num_channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &num_channels, 0);
    if (!data) {
        HG_LOG_ERROR("Failed to load skybox texture {}", path);
        return;
    }

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (u32)face, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}

} // namespace Hydrogen