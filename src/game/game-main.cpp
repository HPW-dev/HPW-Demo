#include <iostream>
#include "game-app.hpp"
#include "util/error.hpp"

int main(int argc, char *argv[]) {
  try {
    Game_app app(argc, argv);
    app.run();
  } catch (CN<std::exception> ex) {
    std::cerr << "C++ error: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  } catch (CN<hpw::Error> err) {
    std::cerr << "HPW error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Unknown error" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
