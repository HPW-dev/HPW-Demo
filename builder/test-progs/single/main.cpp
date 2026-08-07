#include <stdexcept>
#include <fstream>

int main(int argc, char** argv) {
  if (argc <= 1)
    throw std::runtime_error("Usage: single.exe <path_to_save>");
  
  std::ofstream f(argv[1]);
  f << "single .cpp compilation test";
}
