/// @file тестовая программа проверяющая что сборка и C++ компиляторы настроены и работают
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

int main(int argc, char** argv) {
  if (!argv) {
    std::cerr << "argv is empty\n";
    return EXIT_FAILURE;
  }

  if (!argv[1]) {
    std::cerr << "argv[1] is empty\n";
    return EXIT_FAILURE;
  }

  if (argc != 2) {
    std::cerr << "validation value needed\n";
    return EXIT_FAILURE;
  }

  try {
    std::ofstream file(".tmp/empty-prog-question.txt");
    auto val = std::string(argv[1]);
    
    if (val.empty()) {
      std::cerr << "validation value is empty\n";
      return EXIT_FAILURE;
    }

    file << val;
  } catch (...) {
    std::cerr << "question file writing error\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
