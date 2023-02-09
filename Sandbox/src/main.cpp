#include "Hydrogen/hydrogen.h"

class Sandbox : public Hydrogen::Application {
  public:
    Sandbox(int width, int height, std::string&& title)
        : Hydrogen::Application(width, height, std::move(title)),
          // m_model("../../Sandbox/assets/models/backpack/backpack.obj", true)
          m_model("../../Sandbox/assets/models/john_117/scene.gltf", false)
    {
        bind_event_callback_func(Hydrogen::EventType::MouseMoved, BIND_EVENT_FUNC(on_mouse_moved));
        bind_event_callback_func(Hydrogen::EventType::KeyPressed, BIND_EVENT_FUNC(on_key_pressed));
        bind_event_callback_func(Hydrogen::EventType::MouseScrolled, BIND_EVENT_FUNC(on_mouse_scrolled));
        bind_event_callback_func(Hydrogen::EventType::WindowResize, BIND_EVENT_FUNC(on_window_resized));

        m_camera_position = {0.0f, 0.0f, 4.0f};

        float ratio = float(get_window()->get_width()) / float(get_window()->get_height());
        m_camera = Hydrogen::PerspectiveCamera(glm::radians(60.0f), ratio, 0.1f, 100.0f);
        m_camera.set_position(m_camera_position);

        auto skybox_components = Hydrogen::Skybox::Components{
            .right = "posx.jpg",
            .left = "negx.jpg",
            .top = "posy.jpg",
            .bottom = "negy.jpg",
            .front = "posz.jpg",
            .back = "negz.jpg",
        };
        m_skybox = new Hydrogen::Skybox(skybox_components, "../../Sandbox/assets/Meadow_Skybox/");
        Hydrogen::Renderer3D::set_skybox(m_skybox);

        HG_LOG_INFO("Finished startup...");
    }

    ~Sandbox() {
        delete m_skybox;
    }

    void on_update([[maybe_unused]] double ts) override {
        Hydrogen::Renderer3D::begin_scene(m_camera);

        {
            auto light = Hydrogen::Light{
                .position = {0.0f, 1.0f, 2.0f},

                .constant = 1.0f,
                .linear = 0.09f,
                .quadratic = 0.032f,

                .ambient = {0.2f, 0.2f, 0.2f},
                .diffuse = {1.0f, 1.0f, 1.0f},
                .specular = {1.0f, 1.0f, 1.0f}
            };
            Hydrogen::Renderer3D::add_light_source(light);
        }
        {
            auto light = Hydrogen::Light{
                .position = {-2.0f, -1.0f, -2.0f},

                .constant = 1.0f,
                .linear = 0.09f,
                .quadratic = 0.032f,

                .ambient = {0.2f, 0.2f, 0.2f},
                .diffuse = {1.0f, 1.0f, 1.0f},
                .specular = {1.0f, 1.0f, 1.0f}
            };
            Hydrogen::Renderer3D::add_light_source(light);
        }

        Hydrogen::Renderer3D::draw_model(m_model, {-1.0f, -2.0f, -4.0f}, {1.0f, 1.0f, 1.0f});
        // Hydrogen::Renderer3D::draw_model(m_model, {5.0f, 2.0f, 1.0f}, {0.75f, 0.75f, 0.75f});

        Hydrogen::Renderer3D::end_scene();
    }

    void on_mouse_moved(Hydrogen::Event& event) {
        auto mouse_moved = dynamic_cast<Hydrogen::MouseMovedEvent&>(event);

        double x = mouse_moved.get_xpos();
        double y = mouse_moved.get_ypos();

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

    void on_key_pressed(Hydrogen::Event& event) {
        auto key_pressed = dynamic_cast<Hydrogen::KeyPressedEvent&>(event);

        if (key_pressed.get_key() == Hydrogen::Key::W) {
            m_camera_position += glm::vec3({0.0f, 0.0f, -1.0f});
            m_camera.set_position(m_camera_position);
        } else if (key_pressed.get_key() == Hydrogen::Key::S) {
            m_camera_position += glm::vec3({0.0f, 0.0f, 1.0f});
            m_camera.set_position(m_camera_position);
        } else if (key_pressed.get_key() == Hydrogen::Key::A) {
            m_camera_position += glm::vec3({-1.0f, 0.0f, 0.0f});
            m_camera.set_position(m_camera_position);
        } else if (key_pressed.get_key() == Hydrogen::Key::D) {
            m_camera_position += glm::vec3({1.0f, 0.0f, 0.0f});
            m_camera.set_position(m_camera_position);
        } else if (key_pressed.get_key() == Hydrogen::Key::Q) {
            m_camera_position += glm::vec3({0.0f, -1.0f, 0.0f});
            m_camera.set_position(m_camera_position);
        } else if (key_pressed.get_key() == Hydrogen::Key::E) {
            m_camera_position += glm::vec3({0.0f, 1.0f, 0.0f});
            m_camera.set_position(m_camera_position);
        }
    }

    void on_mouse_scrolled(Hydrogen::Event& event) {
        auto scrolled = dynamic_cast<Hydrogen::MouseScrolledEvent&>(event);

        double yoffset = scrolled.get_yoffset();

        float fov = glm::degrees(m_camera.get_fov());
        fov -= (float)yoffset;

        fov = std::max(fov, 0.0f);
        fov = std::min(fov, 120.0f);

        m_camera.set_fov(glm::radians(fov));
    }

    void on_window_resized(Hydrogen::Event& event) {
        auto resized = dynamic_cast<Hydrogen::WindowResizeEvent&>(event);

        int width = resized.get_width();
        int height = resized.get_height();

        float aspect = (float)width / (float)height;
        m_camera.set_aspect_ratio(aspect);
    }

  private:
    Hydrogen::PerspectiveCamera m_camera;
    glm::vec2 m_mouse_position{};
    glm::vec3 m_camera_position;

    Hydrogen::Model m_model;
    Hydrogen::Skybox* m_skybox;
};

int main() {
    Sandbox sandbox = Sandbox(1280, 960, "Sandbox");
    sandbox.run();
    return 0;
}