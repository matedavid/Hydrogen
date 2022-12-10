#include "window.h"

#include <iostream>

namespace Hydrogen {

Window::Window(int width, int height, const std::string& title) {
    //
    // Setup GLFW
    //
    if (!glfwInit()) {
        std::cerr << "Error initializing glfw\n";
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Error creating window\n";
        glfwTerminate();
    }
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
    }

    // Set VSync
    glfwSwapInterval(1);
    glViewport(0, 0, width, height);

    //
    // Window data
    //
    m_data.width = width;
    m_data.height = height;

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowUserPointer(m_window, &m_data);

    m_data.event_callback = BIND_EVENT_FUNC(Window::on_event);

    //
    // Event callbacks
    //

    // Window events
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        auto data = (WindowData*)glfwGetWindowUserPointer(window);
        data->width = width;
        data->height = height;

        WindowResizeEvent event(width, height);
        data->event_callback(event);
    });

    // Mouse events
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
        auto data = (WindowData*)glfwGetWindowUserPointer(window);

        MouseMovedEvent event(xpos, ypos);
        data->event_callback(event);
    });

    // Mouse button events
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
        auto data = (WindowData*)glfwGetWindowUserPointer(window);

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
            auto data = (WindowData*)glfwGetWindowUserPointer(window);
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

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::should_close() const {
    return glfwWindowShouldClose(m_window);
}

void Window::on_update() const {
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Window::bind_event_func(EventType event, EventCallbackFunc func) {
    m_event_callback[event] = std::move(func);
}

void Window::on_event(Event& event) {
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

} // namespace Hydrogen
