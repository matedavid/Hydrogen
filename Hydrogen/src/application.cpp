#include "application.h"

#include <utility>

namespace Hydrogen {

Application::Application(int width, int height, const std::string& title) {
    m_window = new Window(width, height, title);
    Renderer::init();

    m_instance = this;
}

Application::~Application() {
    Renderer::free();
    delete m_window;
    m_instance = nullptr;
}

void Application::run() {
    double last_time = glfwGetTime();

    while (!m_window->should_close()) {
        double current_time = glfwGetTime();
        double ts = current_time - last_time;
        last_time = current_time;

        Renderer::clear(glm::vec3(0.f, 0.f, 0.f));

        on_update(ts);

        m_window->on_update();
    }
}

void Application::bind_event_callback_func(EventType event, EventCallbackFunc func) {
    m_window->bind_event_func(event, std::move(func));
}

}