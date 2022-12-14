#include "Hydrogen/hydrogen.h"

class Sandbox : public Hydrogen::Application {
  public:
    Sandbox(int width, int height, const std::string& title)
        : Hydrogen::Application(width, height, title)
    {
        bind_event_callback_func(Hydrogen::EventType::MouseMoved, BIND_EVENT_FUNC(on_mouse_moved));

        m_camera.set_position({0.0f, 0.0f, 0.0f});
        m_camera = Hydrogen::OrthographicCamera(-1.0f, 1.0f, -1.0f, 1.0f, -2.5f, 10.0f);

        m_texture = new Hydrogen::Texture("../../Hydrogen/assets/textures/smiley.png");
    }

    void on_update(double ts) override {
        /*
        Hydrogen::Renderer3D::begin_scene(m_camera);
        Hydrogen::Renderer3D::draw_cube({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, glm::vec3(0.2f, 0.3f, 0.2f));
        Hydrogen::Renderer3D::end_scene();
        */

        Hydrogen::Renderer2D::begin_scene(m_camera);
        Hydrogen::Renderer2D::draw_quad({0.0f, 0.0f}, {1.0f, 1.0f}, m_texture);
        // Hydrogen::Renderer2D::draw_quad({0.0f, 0.0f}, {1.0f, 1.0f}, glm::vec3(0.2f, 0.3f, 0.2f));
        Hydrogen::Renderer2D::end_scene();

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
    Hydrogen::OrthographicCamera m_camera;
    glm::vec2 m_mouse_position{};

    Hydrogen::Texture* m_texture;
};

int main() {
    Sandbox sandbox = Sandbox(800, 600, "Sandbox");
    sandbox.run();
    return 0;
}