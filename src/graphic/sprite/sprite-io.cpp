#include <omp.h>
#include <cassert>
#include <utility>
#include "sprite-io.hpp"
#include "graphic/image/palette.hpp"
#include "graphic/image/image.hpp"
#include "sprite.hpp"
//#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "util/str-util.hpp"
#include "util/file/file.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "graphic/util/convert.hpp"

/** делает контур картинки жирнее
Нужен для картинки в rotsprite, чтобы при повороте не было артефактов */
inline void expand_contour(Image& image, cr<Image> mask) {
  #pragma omp parallel for simd schedule(static, 4) collapse(2)
  cfor (y, image.Y)
  cfor (x, image.X) {
    if (mask(x, y) == Pal8::mask_visible) {
      if (mask.get(x-1, y) == Pal8::mask_invisible)
        image.set(x-1, y, image(x, y));
      if (mask.get(x+1, y) == Pal8::mask_invisible)
        image.set(x+1, y, image(x, y));
      if (mask.get(x, y-1) == Pal8::mask_invisible)
        image.set(x, y-1, image(x, y));
      if (mask.get(x, y+1) == Pal8::mask_invisible)
        image.set(x, y+1, image(x, y));
    }
  } // for y, x
} // expand_contour

inline void _data_to_sprite(Sprite &dst, cr<File> file) {
  iferror(file.data.empty(), "_data_to_sprite: file is empty");
  // декодирование:
  int x, y;
  int comp; // сколько цветовых каналов
  stbi_uc *decoded = stbi_load_from_memory( scast<cp<stbi_uc>>(file.data.data()),
    file.data.size(), &x, &y, &comp, STBI_rgb_alpha );
  iferror( !decoded, "_data_to_image: image data is not decoded. File: \"" << file.get_path() << "\"");
// переносим данные в спрайт:
  Image image(x, y);
  Image mask(x, y, Pal8::mask_invisible); // полностью прозрачная маска
  // привязать пути
  auto file_path = file.get_path();
  image.set_path(file_path);
  mask.set_path(file_path);
  // сконвертировать цвета
  int rgba_index = 0;
  for (int i = 0; i < x * y; ++i) {
    auto r = decoded[rgba_index + 0];
    auto g = decoded[rgba_index + 1];
    auto b = decoded[rgba_index + 2];
    auto a = decoded[rgba_index + 3];
    rgba_index += 4;
    Pal8 col = desaturate_average(r, g, b);
    image.fast_set(i, col, {});
    if (a > 127)
      mask.fast_set(i, Pal8::mask_visible, {}); // не прозрачный пиксель
  } // for size
  stbi_image_free(decoded);
  expand_contour(image, mask);
  dst.move_image(std::move(image));
  dst.move_mask(std::move(mask));
} // _data_to_sprite

void load(Sprite &dst, cr<File> file) {
  hpw_log("Sprite.load_file \"" + file.get_path() + "\"\n", Log_stream::debug);
  _data_to_sprite(dst, file);
} // load

void load(Sprite &dst, cr<Str> name) {
  hpw_log("Sprite.load_file(F) \"" + name + "\"\n", Log_stream::debug);
  File file {mem_from_file(name), name};
  _data_to_sprite(dst, file);
} // load

void load(cr<File> file, Sprite &dst) {
  iferror( file.data.empty(), "load sprite(M): file is empty");
  _data_to_sprite(dst, file);
}

void save(cr<Sprite> src, Str file_name) {
  assert(src);
  assert(!file_name.empty());
  
  conv_sep(file_name);
  hpw_log("сохранение спрайта \"" + file_name + "\"\n");

  // pal8 to rgb24
  constexpr uint comp = 4;
  Vector<uint8_t> rgba_p(src.size() * comp);
  cfor (i, src.size()) {
    cauto col = to_palette_rgb24( (src.image())[i] );
    cauto mask = (src.mask())[i];
    cauto rgba_index = i * comp;
    rgba_p[rgba_index + 0] = col.r;
    rgba_p[rgba_index + 1] = col.g;
    rgba_p[rgba_index + 2] = col.b;
    rgba_p[rgba_index + 3] = (mask == Pal8::mask_visible ? 255 : 0);
  }
  
  auto ok = stbi_write_png(file_name.c_str(), src.X(), src.Y(),
    STBI_rgb_alpha, rgba_p.data(), 0);
  iferror(!ok, "не удалось сохранить спрайт: \"" << file_name <<"\"");
} // save
