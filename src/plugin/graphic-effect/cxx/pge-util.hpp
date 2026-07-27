#pragma once

struct context_t;
struct result_t;
struct Pal8;

NOT_EXPORTED bool check_params(const context_t* context, result_t* result);
// получить пиксель картинки быстро без проверок
NOT_EXPORTED Pal8& get_pixel_fast(Pal8 image[], const int x, const int y, const int width);
// получить пиксель картинки с проверками
NOT_EXPORTED Pal8 get_pixel_safe(const Pal8 image[], const int x, const int y,
  const int width, const int height);
// записать пиксель картинки быстро без проверок
NOT_EXPORTED void set_pixel_fast(Pal8 image[], const int x, const int y, const int width,
  const Pal8 val);
// записать пиксель картинки с проверками
NOT_EXPORTED void set_pixel_safe(Pal8 image[], const int x, const int y,
  const int width, const int height, const Pal8 val);