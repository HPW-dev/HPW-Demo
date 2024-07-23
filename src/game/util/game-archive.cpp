#include <cassert>
#include "game-archive.hpp"
#include "game/core/common.hpp"
#include "game/util/config.hpp"
#include "util/file/yaml.hpp"

void init_archive() {
  return_if (hpw::archive);
  assert(hpw::config);
  auto data_path = (*hpw::config)["path"].get_str("data", "data.zip");
  init_unique(hpw::archive, hpw::cur_dir + data_path);
}
