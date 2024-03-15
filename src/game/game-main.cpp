#include "game-app.hpp"

int main(int argc, char *argv[]) {
  Game_app app(argc, argv);
  app.run();
  return EXIT_SUCCESS;
}
