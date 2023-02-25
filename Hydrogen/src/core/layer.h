#pragma once

#include "core.h"

namespace Hydrogen {

class HG_API Layer {
  public:
    virtual ~Layer() {}

    virtual void on_update([[maybe_unused]] double ts) = 0;

    // Events
    virtual void on_window_resize(WindowResizeEvent&) {}
    virtual void on_mouse_moved(MouseMovedEvent&) {}
    virtual void on_mouse_button_pressed(MousePressedEvent&) {}
    virtual void on_mouse_button_released(MouseReleasedEvent&) {}
    virtual void on_mouse_scrolled(MouseScrolledEvent&) {}
    virtual void on_key_pressed(KeyPressedEvent&) {}
    virtual void on_key_released(KeyReleasedEvent&) {}
    virtual void on_key_repeated(KeyRepeatEvent&) {}
};

} // namespace Hydrogen
