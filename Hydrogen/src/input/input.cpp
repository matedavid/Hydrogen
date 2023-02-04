#include "input.h"

#include "core/application.h"

#include "GLFW/glfw3.h"

namespace Hydrogen {

bool Input::is_key_pressed(Key key) {
    auto* window = Application::get()->get_window()->get_native_window();

    i32 glfw_key = static_cast<i32>(key);
    i32 status = glfwGetKey(window, glfw_key);
    return status == GLFW_PRESS || status == GLFW_REPEAT;
}

bool Input::is_mouse_button_pressed(MouseButton button) {
    auto* window = Application::get()->get_window()->get_native_window();

    i32 glfw_button = static_cast<i32>(button);
    i32 status = glfwGetMouseButton(window, glfw_button);
    return status == GLFW_PRESS;
}

} // namespace Hydrogen
