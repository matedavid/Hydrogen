#include "application.h"

#include <utility>
#include <ranges>

#include "renderer/renderer_api.h"
#include "systems/shader_system.h"
#include "systems/texture_system.h"

namespace Hydrogen {

Application::Application(i32 width, i32 height, std::string&& title)
    : m_window(width, height, std::move(title)) {
    m_window.add_event_callback_function([&](Event& event) { on_event(event); });

    ShaderSystem::init();
    TextureSystem::init();

    Renderer3D::init();

    m_instance = this;
}

Application::~Application() {
    for (auto& layer : m_layers) {
        delete layer;
    }

    Renderer3D::free();

    ShaderSystem::free();
    TextureSystem::free();

    m_instance = nullptr;
}

void Application::run() {
    f64 last_time = m_window.get_current_time();

    while (!m_window.should_close()) {
        f64 current_time = m_window.get_current_time();
        f64 ts = current_time - last_time;
        last_time = current_time;

        RendererAPI::clear(glm::vec3(0.0f, 0.0f, 0.0f));

        for (auto& layer : std::ranges::reverse_view(m_layers)) {
            layer->on_update(ts);
        }

        m_window.on_update();
    }
}

void Application::on_event(Event& event) {
    auto callback = [&](Event& evnt, Layer* layer) {
        switch (evnt.get_type()) {
            case EventType::WindowResize:
                layer->on_window_resize(dynamic_cast<WindowResizeEvent&>(evnt));
                break;
            case EventType::MouseMoved:
                layer->on_mouse_moved(dynamic_cast<MouseMovedEvent&>(evnt));
                break;
            case EventType::MouseButtonPressed:
                layer->on_mouse_button_pressed(dynamic_cast<MousePressedEvent&>(evnt));
                break;
            case EventType::MouseButtonReleased:
                layer->on_mouse_button_released(dynamic_cast<MouseReleasedEvent&>(evnt));
                break;
            case EventType::MouseScrolled:
                layer->on_mouse_scrolled(dynamic_cast<MouseScrolledEvent&>(evnt));
                break;
            case EventType::KeyPressed:
                layer->on_key_pressed(dynamic_cast<KeyPressedEvent&>(evnt));
                break;
            case EventType::KeyReleased:
                layer->on_key_released(dynamic_cast<KeyReleasedEvent&>(evnt));
                break;
            case EventType::KeyRepeated:
                layer->on_key_repeated(dynamic_cast<KeyRepeatEvent&>(evnt));
                break;
            case EventType::None:
            default:
                break;
        }
    };

    for (auto& layer : m_layers) {
        if (event.handled)
            return;

        callback(event, layer);
    }
}

} // namespace Hydrogen