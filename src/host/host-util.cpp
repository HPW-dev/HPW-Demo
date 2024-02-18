#include <cassert>
#include "stb/stb_image.h"
#include "host-util.hpp"
#include "command.hpp"
#include "game/game-core.hpp"
#include "game/game-sync.hpp"
#include "game/util/game-archive.hpp"
#include "util/str-util.hpp"
#include "util/file/archive.hpp"
#include "util/error.hpp"
#include "graphic/util/convert.hpp"
#include "graphic/image/color.hpp"

static Vector<float> m_ogl_palette {};

void set_target_ups(int new_ups) {
  assert(new_ups > 0);
  hpw::target_ups = new_ups;
  hpw::target_update_time = 1.0 / hpw::target_ups;
}

Vector<float> load_ogl_palette(CN<Str> fname) {
  // загрузка png
  int x, y;
  int comp; // сколько цветовых каналов
  assert(hpw::archive);
  cauto mem = hpw::archive->get_file(fname);
  cauto mem_data = cptr2ptr<CP<stbi_uc>>(mem.data.data());
  auto decoded = stbi_load_from_memory(mem_data, mem.data.size(), &x, &y, &comp, STBI_rgb);
  iferror( !decoded, "image data is not decoded");
  iferror(x < 256, "ширина картинки должна быть не меньше 256");

  // rgb -> palette array
  Vector<float> ogl_pal;
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
  return ogl_pal;
} // load_ogl_palette

Rgb24 to_palette_rgb24(const Pal8 x) {
  if ( !m_ogl_palette.empty())
    return Rgb24 {
      scast<int>(m_ogl_palette.at(x.val * 3 + 0) * 255.0),
      scast<int>(m_ogl_palette.at(x.val * 3 + 1) * 255.0),
      scast<int>(m_ogl_palette.at(x.val * 3 + 2) * 255.0)
    };
  return to_rgb24(x);
}
