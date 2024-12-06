#include "util/macro.hpp"
#include "util/file/archive.hpp"
#include "util/path.hpp"
#include "util/log.hpp"

void print_raw(cr<Bytes> raw) {
  if (raw.empty())
    return;
  Str str(raw.begin(), raw.end());
  hpw_log(str << "\n");
}

int main(int /*argc*/, char *argv[]) {
  auto launch_dir {launch_dir_from_argv0(argv)};
  Archive archive(launch_dir + "../test/archive test/test.zip");

  hpw_log("\nfiles:\n");
  auto names {archive.get_all_names()};
  for (crauto name: names)
    hpw_log("\"" << name << "\"\n")
  hpw_log("\n");

  auto file_1 {archive.get_file("file 1.txt")};
  print_raw(file_1.data);
  auto file_2 {archive.get_file("file 2.txt")};
  print_raw(file_2.data);
}
