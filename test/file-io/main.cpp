#include "util/log.hpp"
#include "util/str.hpp"
#include "util/file/file-io.hpp"

void write_test() {
  const Str fname = "delme.dat";
  hpw_log("test writing to \"" + fname + "\"\n");
}

void read_test() {
  const Str fname = "delme.dat";
  hpw_log("test reading from \"" + fname + "\"\n");
}

int main() {
  hpw_log("file I/O test\n");
  write_test();
  read_test();
}
