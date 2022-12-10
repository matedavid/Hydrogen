#pragma once

#include "core.h"

#include <string>
#include <unordered_map>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "events.h"

namespace Hydrogen {

using EventCallbackFunc = std::function<void(Event&)>;

#define BIND_EVENT_FUNC(func)                                     \
    [this](auto&&... args) -> decltype(auto) {                    \
        return this->func(std::forward<decltype(args)>(args)...); \
    }

struct WindowData {
    int width;
    int height;

    EventCallbackFunc event_callback;
};

class Window {
  public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool should_close() const;
    void on_update() const;

    GLFWwindow* get_native_window() const { return m_window; }
    void bind_event_func(EventType event, EventCallbackFunc func);

  private:
    GLFWwindow* m_window;
    WindowData m_data;

    std::unordered_map<EventType, EventCallbackFunc> m_event_callback;

    void on_event(Event& event);
};

} // namespace Hydrogen
