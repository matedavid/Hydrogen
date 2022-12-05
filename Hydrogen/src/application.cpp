#include "application.h"

namespace Hydrogen {

Application::Application(int width, int height, const std::string& title) {
    m_data.width = width;
    m_data.height = height;
    // TODO: m_data.mouse_position = vec2(0.f, 0.f);

    m_window = Renderer::init(width, height, title);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowUserPointer(m_window, &m_data);

    m_data.event_callback = BIND_EVENT_FUNC(Application::on_event);

    // Window events
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        auto data = (ApplicationData*)glfwGetWindowUserPointer(window);
        data->width = width;
        data->height = height;

        WindowResizeEvent event(width, height);
        data->event_callback(event);
    });

    // Mouse events
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
        auto data = (ApplicationData*)glfwGetWindowUserPointer(window);

        MouseMovedEvent event(xpos, ypos);
        data->event_callback(event);
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
        auto data = (ApplicationData*)glfwGetWindowUserPointer(window);

        if (action == GLFW_PRESS) {
            MousePressedEvent event(button, mods);
            data->event_callback(event);
        } else if (action == GLFW_RELEASE) {
            MouseReleasedEvent event(button, mods);
            data->event_callback(event);
        }
    });

    // Keyboard events
    glfwSetKeyCallback(
        m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            auto data = (ApplicationData*)glfwGetWindowUserPointer(window);
            if (action == GLFW_PRESS) {
                KeyPressedEvent event(key, scancode, mods);
                data->event_callback(event);
            } else if (action == GLFW_RELEASE) {
                KeyReleasedEvent event(key, scancode, mods);
                data->event_callback(event);
            } else if (action == GLFW_REPEAT) {
                KeyRepeatEvent event(key, scancode, mods);
                data->event_callback(event);
            }
        });
}

Application::~Application() {
    Renderer::free(m_window);
}

void Application::run() {
    double last_time = glfwGetTime();

    while (!glfwWindowShouldClose(m_window)) {
        double current_time = glfwGetTime();
        double ts = current_time - last_time;
        last_time = current_time;

        Renderer::clear(glm::vec3(0.f, 0.f, 0.f));

        on_update(ts);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Application::on_event(Event& event) {
    if (m_event_callback.find(event.get_type()) != m_event_callback.end()) {
        m_event_callback[event.get_type()](event);
        return;
    }

    // Default callback functions
    switch (event.get_type()) {
        case EventType::WindowResize: {
            auto resize = dynamic_cast<WindowResizeEvent&>(event);
            glfwSetWindowSize(m_window, resize.get_width(), resize.get_height());
            break;
        }
        default:
            break;
    }
}

void Application::bind_event_callback_func(EventType event, EventCallbackFunc func) {
    m_event_callback[event] = std::move(func);
}

}