#pragma once

#include "core.h"

#include "renderer/renderer.h"
#include "window.h"

namespace Hydrogen {

class HG_API Application {
  public:
    Application(int width, int height, const std::string& title);
    ~Application();

    void run();
    virtual void on_update(double ts) = 0;

    void bind_event_callback_func(EventType event, EventCallbackFunc func);

  private:
    Window* m_window;
};

} // namespace Hydrogen