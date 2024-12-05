#include <cassert>
#include "game-archive.hpp"
#include "game/core/common.hpp"
#include "game/util/config.hpp"
#include "util/file/yaml.hpp"
#include "util/log.hpp"
#include "util/error.hpp"

void init_archive() {
  assert(hpw::config);

  try {
    auto data_path = hpw::cur_dir + (*hpw::config)["path"].get_str("data", hpw::data_path);
    init_unique(hpw::archive, data_path);

    hpw_log("ресурсы будут грузиться из архива \"" + hpw::archive->get_path() + "\"\n",
      Log_stream::info);
    return;
  } catch (cr<hpw::Error> err) {
    hpw_log(Str("ошибка при чтении архива игры:\n") + err.what(), Log_stream::debug);
  } catch (...) {
    hpw_log("неизвестная ошибка при чтении архива игры\n", Log_stream::debug);
  }

  hpw_log("ресурсы будут грузиться с диска из папки \"" + hpw::os_resources_dir + "\"\n",
    Log_stream::info);
}
