#pragma once

#include "core.h"

#include "window.h"
#include "renderer/renderer3d.h"

namespace Hydrogen {

class HG_API Application {
  public:
    Application(i32 width, i32 height, std::string&& title);
    ~Application();

    void run();
    virtual void on_update(f64 ts) = 0;

    void bind_event_callback_func(EventType event, EventCallbackFunc func);
    const Window* get_window() const { return m_window; };

    static Application* get() { return m_instance; }

  private:
    Window* m_window;

    inline static Application* m_instance;
};

} // namespace Hydrogen