#pragma once

#include "core.h"

#include "core/window.h"
#include "core/layer.h"
#include "renderer/renderer3d.h"

namespace Hydrogen {

class HG_API Application {
  public:
    Application(i32 width, i32 height, std::string&& title);
    ~Application();

    static Application* instance() { return m_instance; }
    const Window& get_window() const { return m_window; };

    void run();

    template<typename T>
    void register_layer() {
        static_assert(std::is_base_of<Layer, T>::value, "Registered layer must inherit from Layer");

        Layer* layer = new T();
        m_layers.push_back(layer);
    }

  private:
    Window m_window;
    inline static Application* m_instance;

    std::vector<Layer*> m_layers;

    void on_event(Event& event);
};

} // namespace Hydrogen