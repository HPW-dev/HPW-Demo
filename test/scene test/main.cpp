#include <memory>
#include "host/host-glfw.hpp"
#include "scene-test.hpp"

int main(int argc, char *argv[]) {
  auto main_func = new_shared<Scene_test>();
  Host_glfw host(argc, argv, main_func);
  host.execute();
}
