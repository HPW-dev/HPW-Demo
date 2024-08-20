#include <cassert>
#include <cmath>
#include <string_view>
#include "image-to-asci.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/resize.hpp"

[[nodiscard]] inline static Str img_to_grayscale(cr<Image> src,
const uint W, const uint H, cr<std::string_view> PAL) {
  constexpr uint CHAR_Y = 13;
  constexpr uint CHAR_X = 8;
  assert(PAL.size() > 1);

  const uint RESIZE_Y = H * CHAR_Y;
  const uint RESIZE_X = W * CHAR_X;
  assert(RESIZE_Y);
  assert(RESIZE_X);

  Str asci_art;
  asci_art.reserve(W*H);

  cauto resized = resize_bilinear(src, RESIZE_X, RESIZE_Y);

  cfor (y, H) {
    cfor (x, W) {
      const Rect rect(x * CHAR_X, y * CHAR_Y, CHAR_X, CHAR_Y);
      cauto tile = cut(resized, rect);
      assert(tile);
      // вычислить среднюю яркость всех пикселей
      real avr_luma {};
      for (cauto pix: tile)
        avr_luma += pix.to_real();
      avr_luma /= tile.size;

      // конвертирует яркость пикселя в соответствующий по яркости ASCI-символ
      assert(avr_luma >= 0);
      assert(avr_luma <= 1);
      asci_art += PAL.at(std::round(avr_luma * (PAL.size()-1)));
    } // for x -> TILES_X

    asci_art += '\n';
  } // for y -> TILES_Y

  return asci_art;
}

Str to_asci(cr<Image> src, const uint W, const uint H, const Img_to_asci_mode MODE) {
  assert(W);
  assert(H);
  assert(src);

  switch (MODE) {
    default:
    case Img_to_asci_mode::grayscale_large_pal: return img_to_grayscale(src, W, H,
        " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@");
    case Img_to_asci_mode::grayscale_small_pal: return img_to_grayscale(src, W, H, " .,:ilwW");
    case Img_to_asci_mode::grayscale_small_pal_2: return img_to_grayscale(src, W, H, " .:-=+*#%@");
  } // switch MODE

  return {}; // заглушка от ворнингов
}
