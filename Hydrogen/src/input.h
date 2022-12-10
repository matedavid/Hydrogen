#pragma once

#include "core.h"

namespace Hydrogen {

class HG_API Input {
  public:
    static bool is_key_pressed(int keycode);
    static bool is_mouse_button_pressed(int button);
};

}; // namespace Hydrogen
