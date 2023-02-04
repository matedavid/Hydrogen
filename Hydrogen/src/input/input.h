#pragma once

#include "core.h"
#include "keycodes.h"

namespace Hydrogen {

class HG_API Input {
  public:
    static bool is_key_pressed(Key key);
    static bool is_mouse_button_pressed(MouseButton button);
};

}; // namespace Hydrogen
