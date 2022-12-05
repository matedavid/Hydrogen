#include "Hydrogen/hydrogen.h"

class Sandbox : public Hydrogen::Application {
  public:
    Sandbox(int width, int height, const std::string& title)
        : Hydrogen::Application(width, height, title) {}

    void on_update(double ts) override {
        Hydrogen::Renderer::draw_quad(0.0f, 0.0f, 1.0f, 1.0f, glm::vec3(0.2f, 0.3f, 0.2f));
    }
};

int main() {
    Sandbox sandbox = Sandbox(800, 600, "Sandbox");
    sandbox.run();
    return 0;
}