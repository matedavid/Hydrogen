#pragma once

#include "core.h"
#include "input/keycodes.h"

namespace Hydrogen {

enum class EventType {
    None,
    // Window Events
    WindowResize,
    // Mouse Events
    MouseMoved,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseScrolled,
    // Keyboard Events
    KeyPressed,
    KeyReleased,
    KeyRepeated,
};

class HG_API Event {
  public:
    bool handled = false;
    virtual EventType get_type() const = 0;
};

class WindowResizeEvent : public Event {
  public:
    WindowResizeEvent(i32 width, i32 height) : m_width(width), m_height(height) {}

    EventType get_type() const override { return EventType::WindowResize; }

    i32 get_width() const { return m_width; }
    i32 get_height() const { return m_height; }

  private:
    i32 m_width, m_height;
};

class MouseMovedEvent : public Event {
  public:
    MouseMovedEvent(f64 xpos, f64 ypos) : m_xpos(xpos), m_ypos(ypos) {}

    EventType get_type() const override { return EventType::MouseMoved; }

    f64 get_xpos() const { return m_xpos; }
    f64 get_ypos() const { return m_ypos; }

  private:
    f64 m_xpos, m_ypos;
};

class MousePressedEvent : public Event {
  public:
    MousePressedEvent(MouseButton button, i32 mods) : m_button(button), m_mods(mods) {}

    EventType get_type() const override { return EventType::MouseButtonPressed; }

    MouseButton get_button() const { return m_button; }
    i32 get_mods() const { return m_mods; }

  private:
    MouseButton m_button;
    i32 m_mods;
};

class MouseReleasedEvent : public Event {
  public:
    MouseReleasedEvent(MouseButton button, i32 mods) : m_button(button), m_mods(mods) {}

    EventType get_type() const override { return EventType::MouseButtonReleased; }

    MouseButton get_button() const { return m_button; }
    i32 get_mods() const { return m_mods; }

  private:
    MouseButton m_button;
    i32 m_mods;
};

class MouseScrolledEvent : public Event {
  public:
    MouseScrolledEvent(f64 xoffset, f64 yoffset) : m_xoffset(xoffset), m_yoffset(yoffset) {}

    EventType get_type() const override { return EventType::MouseScrolled; }

    f64 get_xoffset() const { return m_xoffset; }
    f64 get_yoffset() const { return m_yoffset; }

    std::pair<f64, f64> get_offset() const { return {m_xoffset, m_yoffset}; }

  private:
    f64 m_xoffset, m_yoffset;
};

class KeyPressedEvent : public Event {
  public:
    KeyPressedEvent(Key key, i32 scancode, i32 mods)
        : m_key(key), m_scancode(scancode), m_mods(mods) {}

    EventType get_type() const override { return EventType::KeyPressed; }

    Key get_key() const { return m_key; }
    i32 get_scancode() const { return m_scancode; }
    i32 get_mods() const { return m_mods; }

  private:
    Key m_key;
    i32 m_scancode, m_mods;
};

class KeyReleasedEvent : public Event {
  public:
    KeyReleasedEvent(Key key, i32 scancode, i32 mods)
        : m_key(key), m_scancode(scancode), m_mods(mods) {}

    EventType get_type() const override { return EventType::KeyReleased; }

    Key get_key() const { return m_key; }
    i32 get_scancode() const { return m_scancode; }
    i32 get_mods() const { return m_mods; }

  private:
    Key m_key;
    i32 m_scancode, m_mods;
};

class KeyRepeatEvent : public Event {
  public:
    KeyRepeatEvent(Key key, i32 scancode, i32 mods)
        : m_key(key), m_scancode(scancode), m_mods(mods) {}

    EventType get_type() const override { return EventType::KeyRepeated; }

    Key get_key() const { return m_key; }
    i32 get_scancode() const { return m_scancode; }
    i32 get_mods() const { return m_mods; }

  private:
    Key m_key;
    i32 m_scancode, m_mods;
};

} // namespace Hydrogen