#include "util/log.hpp"
#include "util/error.hpp"

int main(const int argc, const char* argv[]) {
  iferror(argc < 2, "Need more args.\nUsage: ./code-to-image __code_dir_name__");
}
