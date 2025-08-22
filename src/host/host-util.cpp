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
    "H . P . W",
    ":: H :: P :: W ::",
    "~H~ ~P~ ~W~",
    "-H-P-W-",
    ":H:P:W:",
    "h3.14w",
    "23.06.2025 02:52",
    "H P W",
    "h p w",
    "ХПВ",
    "}{ |\"| \\/\\/",
    "АшПэВэ",
    "ХеПеВе",
    "ХэПэВэ",
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
    "482E502E57h",
    "0x482E502E57",
    "S5966A.1",
    "JAXFALSX",
    "EGAHI",
    "Exifils'p Mfubi Tloia",
    "SGFsaWxvdidzIFBpeGVsIFdvcmxkDQo=",
    "w2=:=@GVD !:I6= (@C=5",
    "CRC32: 7CF8DF53",
    "MD5: EFBDE72C113693F6F033D20CFB78A0F6",
    "SHA-256: 1493F8C2A36C9622B5824E8DB2E73F3B5FCD52D8FA2605325D9AAC5E6DCD6BE0",
    "w]!](",
    "H-P-W",
    ".... .-.-.- .--. .-.-.- .--",
    R"(\u0048\u002E\u0050\u002E\u0057)",
  }; // window_names

  return window_names.at(rndu_fast(window_names.size() - 1));
}
