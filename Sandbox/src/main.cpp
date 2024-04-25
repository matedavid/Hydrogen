#include "Hydrogen/hydrogen.h"

class SandboxLayer : public Hydrogen::Layer {
  public:
    SandboxLayer()
        : m_camera_position(0.0f, 0.0f, 4.0f),
          m_model("../../Sandbox/assets/john_117/scene.gltf", false) {
        const auto width = Hydrogen::Application::instance()->get_window().get_width();
        const auto height = Hydrogen::Application::instance()->get_window().get_height();

        float ratio = float(width) / float(height);
        m_camera = Hydrogen::PerspectiveCamera(glm::radians(60.0f), ratio, 0.1f, 100.0f);
        m_camera.set_position(m_camera_position);

        m_skybox = new Hydrogen::Skybox("../../Sandbox/assets/old_hall_4k.hdr");

        Hydrogen::Renderer3D::set_skybox(m_skybox);

        light_positions = {glm::vec3(-10.0f, 10.0f, 10.0f), glm::vec3(10.0f, 10.0f, 10.0f),
                           glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(10.0f, -10.0f, 10.0f)};

        light_colors = {glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f),
                        glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f)};

        HG_LOG_INFO("Finished startup...");
    }

    ~SandboxLayer() { delete m_skybox; }

    const unsigned int number_rows = 7;
    const unsigned int number_cols = 7;
    const float spacing = 2.5f;

    const Hydrogen::Light base_light = Hydrogen::Light{
        .position = glm::vec3(0.0f, 0.0f, 0.0f),
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f,

        .ambient = {0.2f, 0.2f, 0.2f},
        .diffuse = glm::vec3(1.0f),
        .specular = {1.0f, 1.0f, 1.0f}
    };

    void on_update([[maybe_unused]] double ts) override {
        Hydrogen::Renderer3D::begin_frame(m_camera);

        Hydrogen::Renderer3D::draw_model(m_model, glm::vec3(-0.5f, -2.0f, -4.0f), glm::vec3(1.0f));

        Hydrogen::Renderer3D::end_frame();
    }

    void on_mouse_moved(Hydrogen::MouseMovedEvent& event) override {
        double x = event.get_xpos();
        double y = event.get_ypos();

        if (Hydrogen::Input::is_mouse_button_pressed(Hydrogen::MouseButton::Left)) {
            float x_rotation = 0.0f;
            float y_rotation = 0.0f;

            if (x > m_mouse_position.x) {
                x_rotation -= 0.03f;
            } else if (x < m_mouse_position.x) {
                x_rotation += 0.03f;
            }

            if (y > m_mouse_position.y) {
                y_rotation += 0.03f;
            } else if (y < m_mouse_position.y) {
                y_rotation -= 0.03f;
            }

            m_camera.rotate({x_rotation, y_rotation});
        }

        m_mouse_position = glm::vec2(x, y);
    }

    void on_key_pressed(Hydrogen::KeyPressedEvent& event) override {
        if (event.get_key() == Hydrogen::Key::W) {
            m_camera_position += glm::vec3({0.0f, 0.0f, -1.0f});
        } else if (event.get_key() == Hydrogen::Key::S) {
            m_camera_position += glm::vec3({0.0f, 0.0f, 1.0f});
        } else if (event.get_key() == Hydrogen::Key::A) {
            m_camera_position += glm::vec3({-1.0f, 0.0f, 0.0f});
        } else if (event.get_key() == Hydrogen::Key::D) {
            m_camera_position += glm::vec3({1.0f, 0.0f, 0.0f});
        } else if (event.get_key() == Hydrogen::Key::Q) {
            m_camera_position += glm::vec3({0.0f, -0.5f, 0.0f});
        } else if (event.get_key() == Hydrogen::Key::E) {
            m_camera_position += glm::vec3({0.0f, 0.5f, 0.0f});
        }

        m_camera.set_position(m_camera_position);
    }

    void on_mouse_scrolled(Hydrogen::MouseScrolledEvent& event) override {
        double yoffset = event.get_yoffset();

        float fov = glm::degrees(m_camera.get_fov());
        fov -= (float)yoffset;

        fov = std::max(fov, 0.0f);
        fov = std::min(fov, 120.0f);

        m_camera.set_fov(glm::radians(fov));
    }

    void on_window_resize(Hydrogen::WindowResizeEvent& event) override {
        int width = event.get_width();
        int height = event.get_height();

        float aspect = (float)width / (float)height;
        m_camera.set_aspect_ratio(aspect);
    }

  private:
    Hydrogen::PerspectiveCamera m_camera;
    glm::vec2 m_mouse_position{};
    glm::vec3 m_camera_position;

    Hydrogen::Model m_model;
    Hydrogen::Skybox* m_skybox = nullptr;

    std::vector<glm::vec3> light_positions;
    std::vector<glm::vec3> light_colors;
};

int main() {
    auto app = Hydrogen::Application(1280, 960, "Sandbox");
    app.register_layer<SandboxLayer>();

    app.run();
    return 0;
}
