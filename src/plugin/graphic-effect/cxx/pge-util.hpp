#pragma once

struct context_t;
struct result_t;
struct Pal8;

NOT_EXPORTED bool check_params(const context_t* context, result_t* result);
// получить пиксель картинки быстро без проверок
NOT_EXPORTED Pal8& get_pixel_fast(Pal8 image[], const int x, const int y, const int width);
// получить пиксель картинки быстро без проверок
NOT_EXPORTED void set_pixel_fast(Pal8 image[], const int x, const int y, const int width,
  const Pal8 val);