#include <iomanip>
#include "protownd.hpp"
#include "host/command.hpp"
#include "game/core/canvas.hpp"
#include "game/core/core-window.hpp"
#include "game/core/graphic.hpp"
#include "game/core/common.hpp"
#include "game/util/config.hpp"
#include "game/util/keybits.hpp"
#include "graphic/image/image-io.hpp"
#include "util/math/random.hpp"
#include "util/str-util.hpp"
#include "util/path.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/file/yaml.hpp"

Protownd::Protownd(int argc, char *argv[]): Host(argc, argv) {
  init_unique(graphic::canvas, graphic::width, graphic::height);
  iferror(graphic::canvas->size == 0 || graphic::canvas->size >= 1024*720,
    "canvas bad size: " + n2s(graphic::canvas->size));
  m_w = graphic::canvas->X;
  m_h = graphic::canvas->Y;
}

void Protownd::_gen_palette() {
  constexpr real GRAY_MUL = 1.0 / Pal8::gray_end;
	constexpr real RED_MUL = 1.0 / 31.0;
  m_pal_rgb.resize(3 * 256);
  // генерация палитры HPW:
  for (uint i = 0; i < Pal8::gray_end + 1; ++i) {
    cauto col = i * GRAY_MUL;
    m_pal_rgb[i * 3 + 0] = col;
    m_pal_rgb[i * 3 + 1] = col;
    m_pal_rgb[i * 3 + 2] = col;
  }
  for (uint i = Pal8::gray_end + 1; i < Pal8::white; ++i) {
    cauto col = (i - Pal8::gray_end - 1) * RED_MUL;
    m_pal_rgb[i * 3 + 0] = col;
    m_pal_rgb[i * 3 + 1] = 0;
    m_pal_rgb[i * 3 + 2] = 0;
  }
  // last WHITE color
  m_pal_rgb[255 * 3 + 0] = 1;
  m_pal_rgb[255 * 3 + 1] = 1;
  m_pal_rgb[255 * 3 + 2] = 1;
// показать все цвета палитры:
#if defined(DEBUG) && defined(PALPRINT)
  log_debug << "Palette:";
  log_debug << "INDEX\tR,     G,     B";
  for (int i = 0; i < 256; ++i) {
    log_debug << i << "\t" <<
      n2s(m_pal_rgb[i * 3 + 0], 3) << ", " <<
      n2s(m_pal_rgb[i * 3 + 1], 3) << ", " <<
      n2s(m_pal_rgb[i * 3 + 2], 3);
  }
  log_debug << "\n"; // endl
#endif
} // _gen_palette

void Protownd::reshape(int w, int h) {
  ogl_resize(w, h);
  // окно нулевого размера не надо отображать
  graphic::enable_render = !m_window_ctx.is_bad();  
} // reshape

void Protownd::set_window_pos(int x, int y) {
  m_window_ctx.sx = x;
  m_window_ctx.sy = y;
  // окно нулевого размера не надо отображать
  graphic::enable_render = !m_window_ctx.is_bad();
}

void Protownd::_set_resize_mode(Resize_mode mode) {
  graphic::resize_mode = mode;
  m_window_ctx.mode = mode;
  reshape(graphic::width, graphic::height);
  /* если фулскрин был включён, то сначала выключить его,
  иначе на линуксе экран погаснет */
  if (graphic::fullscreen) {
    hpw::set_fullscreen(false);
    hpw::set_fullscreen(true);
  }
}
