#include <cassert>
#include "font-helper.hpp"
#include "util/log.hpp"
#include "game/core/fonts.hpp"
#include "game/util/game-archive.hpp"
#include "graphic/font/unifont.hpp"
#include "graphic/font/unifont-mono.hpp"

void load_fonts() {
  assert(hpw::archive);
  hpw_log("загрузка шрифтов...\n");
  auto mem = hpw::archive->get_file("resource/font/unifont-13.0.06.ttf");
  init_unique<Unifont>(graphic::font, mem, 16, true);
  init_unique<Unifont>(graphic::font_shop, mem, 32, true);
  init_unique<Unifont_mono>(graphic::system_mono, mem, 8, 16, true);
}
