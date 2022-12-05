#pragma once

#include "core.h"

namespace Hydrogen {

enum class EventType {
    None,
    // Window Events
    WindowResize,
    // Mouse Events
    MouseMoved,
    MouseButtonPressed,
    MouseButtonReleased,
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
    MousePressedEvent(int button, int mods) : m_button(button), m_mods(mods) {}

    EventType get_type() const override { return EventType::MouseButtonPressed; }

    int get_button() const { return m_button; }
    int get_mods() const { return m_mods; }

  private:
    int m_button, m_mods;
};

class MouseReleasedEvent : public Event {
  public:
    MouseReleasedEvent(int button, int mods) : m_button(button), m_mods(mods) {}

    EventType get_type() const override { return EventType::MouseButtonReleased; }

    int get_button() const { return m_button; }
    int get_mods() const { return m_mods; }

  private:
    int m_button, m_mods;
};

class KeyPressedEvent : public Event {
  public:
    KeyPressedEvent(int key, int scancode, int mods)
        : m_key(key), m_scancode(scancode), m_mods(mods) {}

    EventType get_type() const override { return EventType::KeyPressed; }

    int get_key() const { return m_key; }
    int get_scancode() const { return m_scancode; }
    int get_mods() const { return m_mods; }

  private:
    int m_key, m_scancode, m_mods;
};

class KeyReleasedEvent : public Event {
  public:
    KeyReleasedEvent(int key, int scancode, int mods)
        : m_key(key), m_scancode(scancode), m_mods(mods) {}

    EventType get_type() const override { return EventType::KeyReleased; }

    int get_key() const { return m_key; }
    int get_scancode() const { return m_scancode; }
    int get_mods() const { return m_mods; }

  private:
    int m_key, m_scancode, m_mods;
};

class KeyRepeatEvent : public Event {
  public:
    KeyRepeatEvent(int key, int scancode, int mods)
        : m_key(key), m_scancode(scancode), m_mods(mods) {}

    EventType get_type() const override { return EventType::KeyRepeated; }

    int get_key() const { return m_key; }
    int get_scancode() const { return m_scancode; }
    int get_mods() const { return m_mods; }

  private:
    int m_key, m_scancode, m_mods;
};

} // namespace renderer