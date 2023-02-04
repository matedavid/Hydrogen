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
    virtual EventType get_type() const = 0;
};

class WindowResizeEvent : public Event {
  public:
    WindowResizeEvent(int width, int height) : m_width(width), m_height(height) {}

    EventType get_type() const override { return EventType::WindowResize; }

    int get_width() const { return m_width; }
    int get_height() const { return m_height; }

  private:
    int m_width, m_height;
};

class MouseMovedEvent : public Event {
  public:
    MouseMovedEvent(double xpos, double ypos) : m_xpos(xpos), m_ypos(ypos) {}

    EventType get_type() const override { return EventType::MouseMoved; }

    double get_xpos() const { return m_xpos; }
    double get_ypos() const { return m_ypos; }

  private:
    double m_xpos, m_ypos;
};

class MousePressedEvent : public Event {
  public:
    MousePressedEvent(MouseButton button, int mods) : m_button(button), m_mods(mods) {}

    EventType get_type() const override { return EventType::MouseButtonPressed; }

    MouseButton get_button() const { return m_button; }
    int get_mods() const { return m_mods; }

  private:
    MouseButton m_button;
    int m_mods;
};

class MouseReleasedEvent : public Event {
  public:
    MouseReleasedEvent(MouseButton button, int mods) : m_button(button), m_mods(mods) {}

    EventType get_type() const override { return EventType::MouseButtonReleased; }

    MouseButton get_button() const { return m_button; }
    int get_mods() const { return m_mods; }

  private:
    MouseButton m_button;
    int m_mods;
};

class MouseScrolledEvent : public Event {
  public:
    MouseScrolledEvent(double xoffset, double yoffset) : m_xoffset(xoffset), m_yoffset(yoffset) {}

    EventType get_type() const override { return EventType::MouseScrolled; }

    double get_xoffset() const { return m_xoffset; }
    double get_yoffset() const { return m_yoffset; }

    std::pair<double, double> get_offset() const { return {m_xoffset, m_yoffset}; }

  private:
    double m_xoffset, m_yoffset;
};

class KeyPressedEvent : public Event {
  public:
    KeyPressedEvent(Key key, int scancode, int mods)
        : m_key(key), m_scancode(scancode), m_mods(mods) {}

    EventType get_type() const override { return EventType::KeyPressed; }

    Key get_key() const { return m_key; }
    int get_scancode() const { return m_scancode; }
    int get_mods() const { return m_mods; }

  private:
    Key m_key;
    int m_scancode, m_mods;
};

class KeyReleasedEvent : public Event {
  public:
    KeyReleasedEvent(Key key, int scancode, int mods)
        : m_key(key), m_scancode(scancode), m_mods(mods) {}

    EventType get_type() const override { return EventType::KeyReleased; }

    Key get_key() const { return m_key; }
    int get_scancode() const { return m_scancode; }
    int get_mods() const { return m_mods; }

  private:
    Key m_key;
    int m_scancode, m_mods;
};

class KeyRepeatEvent : public Event {
  public:
    KeyRepeatEvent(Key key, int scancode, int mods)
        : m_key(key), m_scancode(scancode), m_mods(mods) {}

    EventType get_type() const override { return EventType::KeyRepeated; }

    Key get_key() const { return m_key; }
    int get_scancode() const { return m_scancode; }
    int get_mods() const { return m_mods; }

  private:
    Key m_key;
    int m_scancode, m_mods;
};

} // namespace Hydrogen