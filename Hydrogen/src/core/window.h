#pragma once

#include "core.h"

#include <string>
#include <unordered_map>
#include <functional>

#include "input/events.h"

// Forward declare
struct GLFWwindow;


namespace Hydrogen {

using EventCallbackFunc = std::function<void(Event&)>;

struct WindowData {
    i32 width;
    i32 height;

    EventCallbackFunc event_callback;
};

class Window {
  public:
    Window(i32 width, i32 height, const std::string& title);
    ~Window();

    bool should_close() const;
    void on_update() const;

    GLFWwindow* get_native_window() const { return m_window; }
    f64 get_current_time() const;

    void add_event_callback_function(std::function<void(Event&)> func);

    i32 get_width() const { return m_data.width; }
    i32 get_height() const { return m_data.height; }

  private:
    GLFWwindow* m_window;
    WindowData m_data;

    std::vector<std::function<void(Event&)>> m_event_callbacks;

    void on_window_event(Event& event);
};

} // namespace Hydrogen
