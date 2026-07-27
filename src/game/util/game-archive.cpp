#include <cassert>
#include "game-archive.hpp"
#include "game/core/common.hpp"
#include "game/util/config.hpp"
#include "util/file/yaml.hpp"
#include "util/log.hpp"
#include "util/error.hpp"

void init_archive() {
#ifndef DISABLE_ARCHIVE
  assert(hpw::config);

  try {
    auto data_path = hpw::cur_dir + (*hpw::config)["path"].get_str("data", hpw::data_path);
    init_unique(hpw::archive, data_path);

    log_info << "ресурсы будут грузиться из архива \"" + hpw::archive->get_path() + "\"";
    return;
  } catch (cr<hpw::Error> err) {
    log_error << "ошибка при чтении архива игры: " << err.what();
  } catch (...) {
    log_error << "неизвестная ошибка при чтении архива игры";
  }
#endif

  log_info << "ресурсы будут грузиться с диска из папки \"" + hpw::os_resources_dir + "\"";
}
