#include "window.h"

#include <iostream>
#include "renderer/renderer_api.h"
#include "GLFW/glfw3.h"

namespace Hydrogen {

Window::Window(i32 width, i32 height, const std::string& title) {
    //
    // Setup GLFW and RendererAPI
    //
    HG_ASSERT(glfwInit(), "Error initializing glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        HG_LOG_ERROR("Error creating window!");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_window);

    bool success = RendererAPI::init((void*)glfwGetProcAddress);
    if (!success) {
        glfwTerminate();
        HG_ASSERT(false, "Failed to initialize Renderer");
    }
    RendererAPI::resize(width, height);

    // Set VSync on
    glfwSwapInterval(1);

    //
    // Window data
    //
    m_data.width = width;
    m_data.height = height;

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowUserPointer(m_window, &m_data);

    m_data.event_callback = [&](Event& event) {
        on_window_event(event);
    };

    //
    // Event callbacks
    //

    // Window events
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, i32 _width, i32 _height) {
        auto data = (WindowData*)glfwGetWindowUserPointer(window);
        data->width = _width;
        data->height = _height;

        glfwSetWindowSize(window, _width, _height);
        RendererAPI::resize(_width, _height);

        WindowResizeEvent event(_width, _height);
        data->event_callback(event);
    });

    // Mouse events
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, f64 xpos, f64 ypos) {
        auto data = (WindowData*)glfwGetWindowUserPointer(window);

        MouseMovedEvent event(xpos, ypos);
        data->event_callback(event);
    });

    // Mouse button events
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, i32 button, i32 action, i32 mods) {
        auto data = (WindowData*)glfwGetWindowUserPointer(window);

        auto mouse_button = static_cast<MouseButton>(button);
        if (action == GLFW_PRESS) {
            MousePressedEvent event(mouse_button, mods);
            data->event_callback(event);
        } else if (action == GLFW_RELEASE) {
            MouseReleasedEvent event(mouse_button, mods);
            data->event_callback(event);
        }
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, f64 xoffset, f64 yoffset) {
        auto data = (WindowData*)glfwGetWindowUserPointer(window);

        MouseScrolledEvent event(xoffset, yoffset);
        data->event_callback(event);
    });

    // Keyboard events
    glfwSetKeyCallback(m_window, [](GLFWwindow* window, i32 _key, i32 scancode, i32 action, i32 mods) {
        auto data = (WindowData*)glfwGetWindowUserPointer(window);

        Key key = static_cast<Key>(_key);
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

f64 Window::get_current_time() const {
    return glfwGetTime();
}

void Window::add_event_callback_function(std::function<void(Event&)> func) {
    m_event_callbacks.emplace_back([=](Event& event) {
        func(event);
    });
}

void Window::on_window_event(Event& event) {
    for (auto& callback_function : m_event_callbacks) {
        callback_function(event);
    }
}

} // namespace Hydrogen
