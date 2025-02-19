#include <filesystem>
#include <cstring>
#include "image-io.hpp"
#include "palette.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "image.hpp"
#include "graphic/util/convert.hpp"
#include "util/str-util.hpp"
#include "util/file/file-io.hpp"
#include "util/log.hpp"
#include "util/error.hpp"

inline void _data_to_image(Image& dst, cr<Bytes> mem) {
  iferror(mem.empty(), "_data_to_image: mem.empty");
  // декодирование:
  int x, y;
  int comp; // сколько цветовых каналов
  auto decoded = stbi_load_from_memory( scast<cp<stbi_uc>>(mem.data()),
    mem.size(), &x, &y, &comp, STBI_rgb);
  iferror( !decoded, "_data_to_image: image data is not decoded");
// переносим данные на растр:
  dst.init(x, y);
  // сконвертировать цвета
  int rgb_index = 0;
  for (int i = 0; i < x * y; ++i) {
    Pal8 col = desaturate_average(
      decoded[rgb_index + 0],
      decoded[rgb_index + 1],
      decoded[rgb_index + 2]);
    rgb_index += 3;
    dst.fast_set(i, col, {});
  } // for size
  stbi_image_free(decoded);
} // _data_to_image

void load(Image& dst, cr<Bytes> mem) {
  log_debug << "Image.load_file(M)";
  _data_to_image(dst, mem);
} // load

void load(Image& dst, cr<Str> name) {
  log_debug << "Image.load_file(F) \"" + name + "\"";
  auto mem = mem_from_file(name);
  _data_to_image(dst, mem);
  dst.set_path(name);
} // load

void save(cr<Image> src, Str name) {
  assert(src);
  assert( !name.empty());
  
  conv_sep(name);
  log_info << "сохранение картинки \"" + name + "\"";
  // pal8 to rgb24
  constexpr uint comp = 3;
  auto rgb24_p = Vector<uint8_t>(src.size * comp);
  cfor (i, src.size) {
    auto col = to_palette_rgb24(src[i]);
    auto rgb24_index = i * comp;
    rgb24_p[rgb24_index + 0] = col.r;
    rgb24_p[rgb24_index + 1] = col.g;
    rgb24_p[rgb24_index + 2] = col.b;
  }
  
  int mem_sz {};
  cauto file_mem = stbi_write_png_to_mem(rgb24_p.data(), 0, src.X, src.Y, STBI_rgb, &mem_sz);
  iferror(!file_mem || mem_sz <= 0, "не удалось сохранить картинку \"" << name <<"\" в память");
  iferror(mem_sz >= 1024 * 1024 * 200, "почему скриншот больше 200 мб?!");

  Bytes image_file_mem(mem_sz);
  std::memcpy(rcast<void*>(image_file_mem.data()), rcast<cp<void>>(file_mem), mem_sz);
  mem_to_file(image_file_mem, name);
}
