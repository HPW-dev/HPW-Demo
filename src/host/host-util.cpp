#include <cassert>
#include "stb/stb_image.h"
#include "host-util.hpp"
#include "command.hpp"
#include "game/core/core.hpp"
#include "game/util/sync.hpp"
#include "game/util/resource-helper.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/math/random.hpp"
#include "util/file/file.hpp"
#include "graphic/util/convert.hpp"
#include "graphic/image/color.hpp"
#include "graphic/image/palette.hpp"

static Vector<GLfloat> m_ogl_palette {};

void set_target_ups(int new_ups) {
  assert(new_ups > 0);
  hpw::target_ups = new_ups;
  hpw::target_tick_time = 1.0 / hpw::target_ups;
}

Vector<GLfloat> load_ogl_palette(cr<Str> fname) {
  // загрузка png
  int x, y;
  int comp; // сколько цветовых каналов
  cauto mem = load_res(fname);
  cauto mem_data = cptr2ptr<cp<stbi_uc>>(mem.data.data());
  auto decoded = stbi_load_from_memory(mem_data, mem.data.size(), &x, &y, &comp, STBI_rgb);
  iferror( !decoded, "image data is not decoded");
  iferror(x < 256, "ширина картинки должна быть не меньше 256");

  // rgb -> palette array
  Vector<GLfloat> ogl_pal;
  int rgb_index = 0;
  for (int i = 0; i < x * y; ++i) {
    constexpr auto mul = (1.0 / 255.0);
    ogl_pal.push_back(decoded[rgb_index + 0] * mul);
    ogl_pal.push_back(decoded[rgb_index + 1] * mul);
    ogl_pal.push_back(decoded[rgb_index + 2] * mul);
    rgb_index += 3;
  }
  stbi_image_free(decoded);
  m_ogl_palette = ogl_pal;
  to_palette_rgb24 = &pal8_to_rgb24_by_file;
  return ogl_pal;
} // load_ogl_palette

Rgb24 pal8_to_rgb24_by_file(const Pal8 x) {
  if ( !m_ogl_palette.empty())
    return Rgb24 {
      scast<int>(m_ogl_palette.at(x.val * 3 + 0) * 255.0),
      scast<int>(m_ogl_palette.at(x.val * 3 + 1) * 255.0),
      scast<int>(m_ogl_palette.at(x.val * 3 + 2) * 255.0)
    };
  return to_rgb24(x);
}

Str rnd_window_name() {
  sconst Strs window_names {
    "HPW",
    "H.P.W",
    "h p w",
    "ХПВ",
    "}{ |\"| \\/\\/",
    "АшПэВэ",
    "ХеПеВе",
    "Игра про бумеранг",
    "Game about UFO",
    "|-| |' \\/\\/",
    "hhhpppwww",
    "HHH PPP WWW",
    ">",
    "V",
    "^",
    "<",
    "\\/",
    "/\\",
    "< < < < < < <",
    "> > > > > > >",
    ":.",
    ".:",
    "48 61 6C 69 6C 6F 76 27 73 20 70 69 78 65 6C 20 77 6F 72 6C 64",
    "48616C696C6F76277320706978656C20776F726C64",
    "стрелялка",
    "ИГРYШKA",
    "Окно с игрой",
    "эпилепсия",
    "Epilepsy",
    "26.83.130.14",
    "int main() { return main(); }",
  }; // window_names

  return window_names.at(rndu_fast(window_names.size() - 1));
}
