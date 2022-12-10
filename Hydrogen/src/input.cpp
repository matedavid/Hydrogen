#include "input.h"

#include "application.h"

namespace Hydrogen {

bool Input::is_key_pressed(int keycode) {
    auto* window = Application::get()->get_window()->get_native_window();
    int status = glfwGetKey(window, keycode);
    return status == GLFW_PRESS || status == GLFW_REPEAT;
}

bool Input::is_mouse_button_pressed(int button) {
    auto* window = Application::get()->get_window()->get_native_window();
    int status = glfwGetMouseButton(window, button);
    return status == GLFW_PRESS;
}

} // namespace Hydrogen
