#include "application.h"

#include <utility>
#include "renderer/renderer_api.h"

namespace Hydrogen {

Application::Application(int width, int height, std::string&& title) {
    m_window = new Window(width, height, std::move(title));
    Renderer2D::init();
    Renderer3D::init();

    m_instance = this;
}

Application::~Application() {
    Renderer2D::free();
    Renderer3D::free();

    delete m_window;
    m_instance = nullptr;
}

void Application::run() {
    double last_time = glfwGetTime();

    while (!m_window->should_close()) {
        double current_time = glfwGetTime();
        double ts = current_time - last_time;
        last_time = current_time;

        RendererAPI::clear(glm::vec3(0.2f, 0.3f, 0.3f));

        on_update(ts);

        m_window->on_update();
    }
}

void Application::bind_event_callback_func(EventType event, EventCallbackFunc func) {
    m_window->bind_event_func(event, std::move(func));
}

}