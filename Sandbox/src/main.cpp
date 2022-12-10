#include "Hydrogen/hydrogen.h"

class Sandbox : public Hydrogen::Application {
  public:
    Sandbox(int width, int height, const std::string& title)
        : Hydrogen::Application(width, height, title) {
        m_shader = Hydrogen::Shader::from_file("../../Hydrogen/assets/vertex.glsl",
                                               "../../Hydrogen/assets/fragment.glsl");
        bind_event_callback_func(Hydrogen::EventType::MouseMoved, BIND_EVENT_FUNC(on_mouse_moved));

        m_camera.set_position({0, 0, 0});
        m_camera = Hydrogen::OrthographicCamera(-2.5, 2.5f, -2.5f, 2.5f, -5, 5);
    }

    void on_update(double ts) override {
        // Hydrogen::Renderer::draw_quad(0.0f, 0.0f, 1.0f, 1.0f, glm::vec3(0.2f, 0.3f, 0.2f));

        // m_shader->set_uniform_mat4(view_projection, "ViewProjection");

        const auto view_projection = m_camera.get_view_projection();

        m_shader->set_uniform_mat4(view_projection, "ViewProjection");
        m_shader->set_uniform_vec3(glm::vec3(0.2f, 0.3f, 0.2f), "Color");

        Hydrogen::Renderer::draw_quad(0.0f, 0.0f, 2.0f, 2.0f, m_shader);
    }

    void on_mouse_moved(Hydrogen::Event& event) {
        auto mouse_moved = dynamic_cast<Hydrogen::MouseMovedEvent&>(event);

        double x = mouse_moved.get_xpos();
        double y = mouse_moved.get_ypos();

        if (Hydrogen::Input::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT)
            && Hydrogen::Input::is_key_pressed(GLFW_KEY_LEFT_CONTROL)) {
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

  private:
    Hydrogen::Shader* m_shader;

    Hydrogen::OrthographicCamera m_camera;
    glm::vec2 m_mouse_position{};
};

int main() {
    Sandbox sandbox = Sandbox(800, 600, "Sandbox");
    sandbox.run();
    return 0;
}