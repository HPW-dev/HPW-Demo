#include <iostream>
#include <cstdlib>
#include "editor.hpp"
#include "util/error.hpp"

int main(int argc, char *argv[]) {
  Editor app(argc, argv);
  app.run();
}
