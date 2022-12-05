#pragma once

#include "core.h"

#include <iostream>
#include <unordered_map>
#include <functional>

#include "renderer.h"
#include "events.h"

namespace Hydrogen {

using EventCallbackFunc = std::function<void(Event&)>;

#define BIND_EVENT_FUNC(func)                                     \
    [this](auto&&... args) -> decltype(auto) {                    \
        return this->func(std::forward<decltype(args)>(args)...); \
    }

struct ApplicationData {
    int width;
    int height;
    // TODO: math::vec2 mouse_position;

    EventCallbackFunc event_callback;
};

class HG_API Application {
  public:
    Application(int width, int height, const std::string& title);
    ~Application();

    void run();
    virtual void on_update(double ts) = 0;

    void on_event(Event& event);
    void bind_event_callback_func(EventType event, EventCallbackFunc func);

  private:
    GLFWwindow* m_window;
    ApplicationData m_data;

    std::unordered_map<EventType, EventCallbackFunc> m_event_callback;
};

}