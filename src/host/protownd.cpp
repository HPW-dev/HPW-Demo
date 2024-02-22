#include <iomanip>
#include "protownd.hpp"
#include "command.hpp"
#include "game/core/canvas.hpp"
#include "game/core/core-window.hpp"
#include "game/core/graphic.hpp"
#include "game/core/common.hpp"
#include "game/util/config.hpp"
#include "game/util/keybits.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/image-io.hpp"
#include "util/math/random.hpp"
#include "util/str-util.hpp"
#include "util/path.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/file/yaml.hpp"

Protownd::Protownd(int argc, char *argv[]): Host(argc, argv) {
  graphic::canvas = new_shared<Image>(graphic::width, graphic::height);
  iferror(graphic::canvas->size == 0 || graphic::canvas->size >= 1024*720,
    "canvas bad size: " + n2s(graphic::canvas->size));
  w_ = graphic::canvas->X;
  h_ = graphic::canvas->Y;
}

void Protownd::save_screenshot() const {
  auto t = std::time(nullptr);
#ifdef LINUX
  struct ::tm lt;
  ::localtime_r(&t, &lt);
#else // WINDOWS
  auto lt = *std::localtime(&t);
#endif

  auto screenshots_dir = hpw::cur_dir + 
    (*hpw::config)["path"].get_str("screenshots", "screenshots") + SEPARATOR;
  std::ostringstream oss;
  oss << screenshots_dir;
  make_dir_if_not_exist(oss.str());
  oss << std::put_time(&lt, "%d-%m-%Y %H-%M-%S");
  oss << "-" + n2s(rndu(100'000)) + ".png";
  save(*graphic::canvas, oss.str());
} // save_screenshot

void Protownd::_gen_palette() {
  constexpr real GRAY_MUL = 1.0 / Pal8::gray_end;
	constexpr real RED_MUL = 1.0 / 31.0;
  pal_rgb.resize(3 * 256);
  // генерация палитры HPW:
  for (uint i = 0; i < Pal8::gray_end + 1; ++i) {
    cauto col = i * GRAY_MUL;
    pal_rgb[i * 3 + 0] = col;
    pal_rgb[i * 3 + 1] = col;
    pal_rgb[i * 3 + 2] = col;
  }
  for (uint i = Pal8::gray_end + 1; i < Pal8::white; ++i) {
    cauto col = (i - Pal8::gray_end - 1) * RED_MUL;
    pal_rgb[i * 3 + 0] = col;
    pal_rgb[i * 3 + 1] = 0;
    pal_rgb[i * 3 + 2] = 0;
  }
  // last WHITE color
  pal_rgb[255 * 3 + 0] = 1;
  pal_rgb[255 * 3 + 1] = 1;
  pal_rgb[255 * 3 + 2] = 1;
// показать все цвета палитры:
#if defined(DEBUG) && defined(PALPRINT)
  hpw_log("Palette:\n");
  hpw_log("INDEX\tR,     G,     B\n");
  for (int i = 0; i < 256; ++i) {
    hpw_log(n2s(i) << "\t" <<
      n2s(pal_rgb[i * 3 + 0], 3) << ", " <<
      n2s(pal_rgb[i * 3 + 1], 3) << ", " <<
      n2s(pal_rgb[i * 3 + 2], 3) << "\n");
  }
  hpw_log("\n"); // endl
#endif
} // _gen_palette

void Protownd::reshape(int w, int h) {
  ogl_resize(w, h);
  // окно нулевого размера не надо отображать
  graphic::enable_render = !window_ctx_.is_bad();  
} // reshape

void Protownd::set_window_pos(int x, int y) {
  window_ctx_.sx = x;
  window_ctx_.sy = y;
  // окно нулевого размера не надо отображать
  graphic::enable_render = !window_ctx_.is_bad();
}

void Protownd::_set_resize_mode(Resize_mode mode) {
  graphic::resize_mode = mode;
  window_ctx_.mode = mode;
  reshape(graphic::width, graphic::height);
  // так надо сделать, чтоб растягивание применилось в фулскрине
  if (graphic::fullscreen) {
    hpw::set_fullscreen(false);
    hpw::set_fullscreen(true);
  }
}
