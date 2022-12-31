#include "Hydrogen/hydrogen.h"

class Sandbox : public Hydrogen::Application {
  public:
    Sandbox(int width, int height, std::string&& title)
        : Hydrogen::Application(width, height, std::move(title)),
          // m_model("../../Hydrogen/assets/models/backpack/backpack.obj", true)
          m_model("../../Hydrogen/assets/models/PistolBeretta/Pistol_Model.obj", false)
    {
        bind_event_callback_func(Hydrogen::EventType::MouseMoved, BIND_EVENT_FUNC(on_mouse_moved));
        bind_event_callback_func(Hydrogen::EventType::KeyPressed, BIND_EVENT_FUNC(on_key_pressed));
        bind_event_callback_func(Hydrogen::EventType::MouseScrolled, BIND_EVENT_FUNC(on_mouse_scrolled));

        m_camera_position = {0.0f, 0.0f, 6.0f};

        float ratio = float(get_window()->get_width()) / float(get_window()->get_height());
        m_camera = Hydrogen::PerspectiveCamera(glm::radians(60.0f), ratio, 0.1f, 100.0f);
        m_camera.set_position(m_camera_position);

        m_shader = Hydrogen::Shader::from_file("../../Hydrogen/assets/model_loading.vert", "../../Hydrogen/assets/model_loading.frag");
    }

    void on_update([[maybe_unused]] double ts) override {
        // Hydrogen::Renderer3D::begin_scene(m_camera);

        m_shader->set_uniform_mat4(m_camera.get_view_projection(), "ViewProjection");

        auto scale_factor = glm::vec3(1.0f);
        m_shader->set_uniform_mat4(glm::scale(glm::mat4(1.0f), scale_factor), "Model");

        // Light
        glm::vec3 light_position = glm::vec3(-3.0f, 3.0f, 0.0f);
        glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
        m_shader->set_uniform_vec3(light_position, "Light.position");
        m_shader->set_uniform_vec3({0.2f, 0.2f, 0.2f}, "Light.ambient");
        m_shader->set_uniform_vec3(light_color, "Light.diffuse");
        m_shader->set_uniform_vec3({1.0f, 1.0f, 1.0f}, "Light.specular");

        // Camera Position
        m_shader->set_uniform_vec3(m_camera_position, "CameraPosition");

        // Draw the model
        m_model.draw(m_shader);

        // Draw the light
        Hydrogen::Renderer3D::begin_scene(m_camera);
        Hydrogen::Renderer3D::draw_cube(light_position, glm::vec3(0.5f, 0.5f, 0.5f), light_color);
        Hydrogen::Renderer3D::end_scene();

//        m_shader->set_uniform_vec3(glm::vec3(0.2f, 0.7f, 0.2f), "Color");
//        Hydrogen::Renderer3D::draw_cube({0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, m_shader);
//
//        m_shader->set_uniform_vec3(glm::vec3(1.0f, 0.5f, 0.31f), "Color");
//        Hydrogen::Renderer3D::draw_cube({2.0f, 0.0f, -2.0f}, {1.0f, 1.0f, 1.0f}, m_shader);
//
//        m_shader->set_uniform_vec3(glm::vec3(0.1f, 0.2f, 0.7f), "Color");
//        Hydrogen::Renderer3D::draw_cube({-2.0f, 2.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, m_shader);

//        m_model.draw(m_shader);

        // Hydrogen::Renderer3D::end_scene();
    }

    void on_mouse_moved(Hydrogen::Event& event) {
        auto mouse_moved = dynamic_cast<Hydrogen::MouseMovedEvent&>(event);

        double x = mouse_moved.get_xpos();
        double y = mouse_moved.get_ypos();

        if (Hydrogen::Input::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
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

        if (key_pressed.get_key() == GLFW_KEY_W) {
            m_camera_position += glm::vec3({0.0f, 0.0f, -1.0f});
            m_camera.set_position(m_camera_position);
        } else if (key_pressed.get_key() == GLFW_KEY_S) {
            m_camera_position += glm::vec3({0.0f, 0.0f, 1.0f});
            m_camera.set_position(m_camera_position);
        } else if (key_pressed.get_key() == GLFW_KEY_A) {
            m_camera_position += glm::vec3({-1.0f, 0.0f, 0.0f});
            m_camera.set_position(m_camera_position);
        } else if (key_pressed.get_key() == GLFW_KEY_D) {
            m_camera_position += glm::vec3({1.0f, 0.0f, 0.0f});
            m_camera.set_position(m_camera_position);
        } else if (key_pressed.get_key() == GLFW_KEY_Q) {
            m_camera_position += glm::vec3({0.0f, -1.0f, 0.0f});
            m_camera.set_position(m_camera_position);
        } else if (key_pressed.get_key() == GLFW_KEY_E) {
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

  private:
    Hydrogen::PerspectiveCamera m_camera;
    glm::vec2 m_mouse_position{};

    Hydrogen::Shader* m_shader;
    glm::vec3 m_camera_position;
    Hydrogen::Model m_model;
};

int main() {
    Sandbox sandbox = Sandbox(800, 600, "Sandbox");
    sandbox.run();
    return 0;
}