#include <cassert>
#include "font-helper.hpp"
#include "util/log.hpp"
#include "game/core/fonts.hpp"
#include "game/util/resource-helper.hpp"
#include "graphic/font/unifont.hpp"
#include "graphic/font/unifont-mono.hpp"
#include "graphic/font/microfont-mono.hpp"

void load_fonts() {
  log_info << "загрузка шрифтов...";
  auto mem = load_res("resource/font/unifont-13.0.06.ttf");
  init_unique<Unifont>(graphic::font, mem, 16, true);
  init_unique<Unifont>(graphic::font_shop, mem, 32, true);
  init_unique<Unifont_mono>(graphic::system_mono, mem, 8, 16, true);
  init_unique<Microfont_mono>(graphic::asci_3x5, "resource/font/microfont mono/downcase.yml");
  init_unique<Microfont_mono>(graphic::asci_5x5, "resource/font/microfont mono/upcase.yml");
}
