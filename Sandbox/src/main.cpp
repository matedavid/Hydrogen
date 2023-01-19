#include "Hydrogen/hydrogen.h"

class Sandbox : public Hydrogen::Application {
  public:
    Sandbox(int width, int height, std::string&& title)
        : Hydrogen::Application(width, height, std::move(title))
    {
        bind_event_callback_func(Hydrogen::EventType::MouseMoved, BIND_EVENT_FUNC(on_mouse_moved));
        bind_event_callback_func(Hydrogen::EventType::KeyPressed, BIND_EVENT_FUNC(on_key_pressed));
        bind_event_callback_func(Hydrogen::EventType::MouseScrolled, BIND_EVENT_FUNC(on_mouse_scrolled));

        m_camera_position = {0.0f, 0.0f, 0.0f};

        float ratio = float(get_window()->get_width()) / float(get_window()->get_height());
        m_camera = Hydrogen::PerspectiveCamera(glm::radians(60.0f), ratio, 0.1f, 100.0f);
        m_camera.set_position(m_camera_position);

        m_shader_red = Hydrogen::Shader::from_file("../../Hydrogen/assets/shaders/uniform_test.vert",
                                                   "../../Hydrogen/assets/shaders/uniform_test_red.frag");

        m_shader_green = Hydrogen::Shader::from_file("../../Hydrogen/assets/shaders/uniform_test.vert",
                                                     "../../Hydrogen/assets/shaders/uniform_test_green.frag");

        m_shader_blue = Hydrogen::Shader::from_file("../../Hydrogen/assets/shaders/uniform_test.vert",
                                                    "../../Hydrogen/assets/shaders/uniform_test_blue.frag");

        unsigned int uniform_block_red   = glGetUniformBlockIndex(m_shader_red->get_id(), "Camera");
        unsigned int uniform_block_green = glGetUniformBlockIndex(m_shader_green->get_id(), "Camera");
        unsigned int uniform_block_blue  = glGetUniformBlockIndex(m_shader_blue->get_id(), "Camera");

        glUniformBlockBinding(m_shader_red->get_id(), uniform_block_red, 0);
        glUniformBlockBinding(m_shader_green->get_id(), uniform_block_green, 0);
        glUniformBlockBinding(m_shader_blue->get_id(), uniform_block_blue, 0);

        // Setup uniform buffer
        m_uniform_buffer = new Hydrogen::UniformBuffer(sizeof(glm::mat4));
        m_uniform_buffer->set_data(0, m_camera.get_view_projection());
    }

    void on_update([[maybe_unused]] double ts) override {
        Hydrogen::Renderer3D::begin_scene(m_camera);

        Hydrogen::Renderer3D::draw_cube({2.0f, 0.0f, -3.0f}, {1.0f, 1.0f, 1.0f}, m_shader_red);
        Hydrogen::Renderer3D::draw_cube({-2.0f, 1.0f, -2.0f}, {1.0f, 1.0f, 1.0f}, m_shader_green);
        Hydrogen::Renderer3D::draw_cube({0.0f, -1.0f, -4.0f}, {1.0f, 1.0f, 1.0f}, m_shader_blue);

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

        // Update uniform buffer data
        m_uniform_buffer->set_data(0, m_camera.get_view_projection());

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

        // Update uniform buffer data
        m_uniform_buffer->set_data(0, m_camera.get_view_projection());
    }

    void on_mouse_scrolled(Hydrogen::Event& event) {
        auto scrolled = dynamic_cast<Hydrogen::MouseScrolledEvent&>(event);

        double yoffset = scrolled.get_yoffset();

        float fov = glm::degrees(m_camera.get_fov());
        fov -= (float)yoffset;

        fov = std::max(fov, 0.0f);
        fov = std::min(fov, 120.0f);

        m_camera.set_fov(glm::radians(fov));

        // Update uniform buffer data
        m_uniform_buffer->set_data(0, m_camera.get_view_projection());
    }

  private:
    Hydrogen::PerspectiveCamera m_camera;
    glm::vec2 m_mouse_position{};

    glm::vec3 m_camera_position;

    Hydrogen::Shader* m_shader_red;
    Hydrogen::Shader* m_shader_green;
    Hydrogen::Shader* m_shader_blue;
    Hydrogen::UniformBuffer* m_uniform_buffer;
};

int main() {
    Sandbox sandbox = Sandbox(800, 600, "Sandbox");
    sandbox.run();
    return 0;
}