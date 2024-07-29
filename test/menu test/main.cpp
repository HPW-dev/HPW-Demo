#include <memory>
#include "host/glfw3/host-glfw.hpp"
#include "menu-test.hpp"

int main(int argc, char *argv[]) {
  auto main_func = new_shared<Menu_test>();
  Host_glfw host(argc, argv, main_func);
  host.execute();
}
