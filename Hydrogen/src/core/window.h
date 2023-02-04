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

#define BIND_EVENT_FUNC(func)                                     \
    [this](auto&&... args) -> decltype(auto) {                    \
        return this->func(std::forward<decltype(args)>(args)...); \
    }

struct WindowData {
    i32 width;
    i32 height;

    EventCallbackFunc event_callback;
};

class Window {
  public:
    Window(i32 width, i32 height, std::string&& title);
    ~Window();

    bool should_close() const;
    void on_update() const;

    GLFWwindow* get_native_window() const { return m_window; }

    f64 get_current_time() const;

    i32 get_width() const { return m_data.width; }
    i32 get_height() const { return m_data.height; }

    void bind_event_func(EventType event, EventCallbackFunc func);

  private:
    GLFWwindow* m_window;
    WindowData m_data;

    std::unordered_map<EventType, EventCallbackFunc> m_event_callback;

    void on_event(Event& event);
};

} // namespace Hydrogen
