#include <cassert>
#include "game-archive.hpp"
#include "game/game-common.hpp"
#include "util/file/archive.hpp"
#include "util/file/yaml.hpp"

void init_archive() {
  return_if (hpw::archive);
  assert(hpw::config);
  auto data_path = (*hpw::config)["path"].get_str("data", "data.zip");
  hpw::archive = new_shared<Archive>(hpw::cur_dir + data_path);
}
