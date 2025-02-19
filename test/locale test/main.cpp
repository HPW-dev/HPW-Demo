#include "util/file/yaml.hpp"
#include "util/file/archive.hpp"
#include "util/path.hpp"
#include "util/log.hpp"
#include "game/locale.hpp"

void test(cr<Str> fname) {
  log_info << ("\nLoading \"" << fname << "\"\n");
  auto yml {load_res("resource/locale/" + fname)};
  Yaml locale_yml(yml);
  hpw::locale->load(locale_yml.get_kv_table());
#define PRINT(str) log_info << ("\t" str ": " \
<< sconv<Str>(hpw::locale->get(str)) << '\n');
  PRINT("info.author");
  PRINT("info.name");
  PRINT("info.version");
  PRINT("error test");
#undef PRINT
}

int main(int argc, char *argv[]) {
  hpw::cur_dir = launch_dir_from_argv0(argv);
  init_archive();
  init_locale();
  test("en.yml");
  test("ru.yml");
}
