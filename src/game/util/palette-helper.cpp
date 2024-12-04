#include <cassert>
#include <ranges>
#include "palette-helper.hpp"
#include "game/core/palette.hpp"
#include "game/util/resource-helper.hpp"
#include "host/command.hpp"
#include "util/str.hpp"
#include "util/rnd-table.hpp"

void randomize_palette() {
  assert(hpw::init_palette_from_archive);

  cauto sprites = get_all_res_names(false);
  cauto filter = [](cr<Str> src)
    { return src.find("resource/image/palettes/") != Str::npos; };
  Rnd_table<Str> palettes(sprites | std::views::filter(filter) | std::ranges::to<Strs>());
  cauto palette_name = palettes.rnd_stable();
  graphic::current_palette_file = palette_name;
  hpw::init_palette_from_archive(palette_name);
}
