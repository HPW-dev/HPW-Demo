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
    auto data_path = (*hpw::config)["path"].get_str("data", hpw::data_path);
    init_unique(hpw::archive, hpw::cur_dir + data_path);
    return;
  } catch (cr<hpw::Error> err) {
    hpw_log(Str("Ошибка при чтении архива игры:\n") + err.what(), Log_stream::warning);
  } catch (...) {
    hpw_log("Неизвестная ошибка при чтении архива игры\n", Log_stream::warning);
  }

  hpw_log("Будет использоваться загрузка ресурсов с диска\n", Log_stream::warning);
}
