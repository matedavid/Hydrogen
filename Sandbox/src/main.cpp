#include "Hydrogen/hydrogen.h"

class Sandbox : public Hydrogen::Application {
  public:
    Sandbox(int width, int height, std::string&& title)
        : Hydrogen::Application(width, height, std::move(title))
    {
        bind_event_callback_func(Hydrogen::EventType::MouseMoved, BIND_EVENT_FUNC(on_mouse_moved));

//        m_camera = Hydrogen::Camera::orthogonal(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
        float ratio = float(get_window()->get_width()) / float(get_window()->get_height());
        m_camera = Hydrogen::Camera::perspective(glm::radians(60.0f), ratio, 0.1f, 100.0f);
        m_camera.set_position({0.0f, 0.0f, 7.0f});

        m_shader = Hydrogen::Shader::from_file("../../Hydrogen/assets/light_vertex.glsl", "../../Hydrogen/assets/light_fragment.glsl");
    }

    void on_update([[maybe_unused]] double ts) override {
        //Hydrogen::Renderer3D::begin_scene(m_camera);

        m_shader->set_uniform_mat4(m_camera.get_view_projection(), "ViewProjection") ;
        m_shader->set_uniform_vec3(glm::vec3(1.0f, 1.0f, 1.0f), "LightColor");

        m_shader->set_uniform_vec3(glm::vec3(0.2f, 0.7f, 0.2f), "Color");
        Hydrogen::Renderer3D::draw_cube({0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, m_shader);

        m_shader->set_uniform_vec3(glm::vec3(1.0f, 0.5f, 0.31f), "Color");
        Hydrogen::Renderer3D::draw_cube({2.0f, 0.0f, -2.0f}, {1.0f, 1.0f, 1.0f}, m_shader);

        m_shader->set_uniform_vec3(glm::vec3(0.1f, 0.2f, 0.7f), "Color");
        Hydrogen::Renderer3D::draw_cube({-2.0f, 2.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, m_shader);

        Hydrogen::Renderer3D::end_scene();
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

  private:
    Hydrogen::Camera m_camera;
    glm::vec2 m_mouse_position{};

    Hydrogen::Shader* m_shader;
};

int main() {
    Sandbox sandbox = Sandbox(800, 600, "Sandbox");
    sandbox.run();
    return 0;
}