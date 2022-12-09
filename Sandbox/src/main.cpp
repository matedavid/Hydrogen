#include "Hydrogen/hydrogen.h"

class Sandbox : public Hydrogen::Application {
  public:
    Sandbox(int width, int height, const std::string& title)
        : Hydrogen::Application(width, height, title) {
        m_shader = Hydrogen::Shader::from_file("../../Hydrogen/assets/vertex.glsl",
                                               "../../Hydrogen/assets/fragment.glsl");
        bind_event_callback_func(Hydrogen::EventType::MouseMoved, BIND_EVENT_FUNC(on_mouse_moved));
    }

    void on_update(double ts) override {
        // Hydrogen::Renderer::draw_quad(0.0f, 0.0f, 1.0f, 1.0f, glm::vec3(0.2f, 0.3f, 0.2f));

        // m_shader->set_uniform_mat4(view_projection, "ViewProjection");

        glm::mat4 view(1.0f);
        view = glm::rotate(view, m_rotation.y, glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, m_rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));

        m_shader->set_uniform_mat4(view, "View");
        m_shader->set_uniform_mat4(glm::mat4(1.0f), "Projection");
        m_shader->set_uniform_vec3(glm::vec3(0.2f, 0.3f, 0.2f), "Color");

        Hydrogen::Renderer::draw_quad(0.0f, 0.0f, 1.0f, 1.0f, m_shader);
    }

    void on_mouse_moved(Hydrogen::Event& event) {
        auto mouse_moved = dynamic_cast<Hydrogen::MouseMovedEvent&>(event);

        double x = mouse_moved.get_xpos();
        double y = mouse_moved.get_ypos();

        if (x > m_mouse_position.x) {
            m_rotation.x -= 0.03f;
        } else if (x < m_mouse_position.x) {
            m_rotation.x += 0.03f;
        }

        if (y > m_mouse_position.y) {
            m_rotation.y += 0.03f;
        } else if (y < m_mouse_position.y) {
            m_rotation.y -= 0.03f;
        }

        m_mouse_position = glm::vec2(x, y);
    }

  private:
    Hydrogen::Shader* m_shader;

    glm::vec2 m_rotation{};
    glm::vec2 m_mouse_position{};
};

int main() {
    Sandbox sandbox = Sandbox(800, 600, "Sandbox");
    sandbox.run();
    return 0;
}