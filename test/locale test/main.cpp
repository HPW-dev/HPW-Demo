#include "util/file/yaml.hpp"
#include "util/file/archive.hpp"
#include "util/path.hpp"
#include "util/log.hpp"
#include "game/locale.hpp"
#include "game/util/game-util.hpp"

void test(cr<Str> fname) {
  hpw_log("\nLoading \"" << fname << "\"\n");
  auto yml {hpw::archive->get_file("resource/locale/" + fname)};
  Yaml locale_yml(yml);
  hpw::locale->load(locale_yml.get_kv_table());
#define PRINT(str) hpw_log("\t" str ": " \
<< sconv<Str>(hpw::locale->get(str)) << '\n');
  PRINT("info.author");
  PRINT("info.name");
  PRINT("info.version");
  PRINT("error test");
#undef PRINT
}

int main(int argc, char *argv[]) {
  hpw::cur_dir = launch_dir_from_argv0(argv[0]);
  init_archive();
  init_locale();
  test("en.yml");
  test("ru.yml");
}
