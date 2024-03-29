#include <assert.h>
#include "hpw-plugin-effect-util.h"
/* Как собрать:
Закиньте hpw-plugin-effect.h и util-файлы в папку с кодом плагина
и соберите командой: gcc -std=c2x -Wall -O2 -shared *.c -o brightness.dll
получившийся .dll/.so файл скиньте в build/plugin/effect/, затем
найдите его в игре в настройках графики. */

NOT_EXPORTED static pal8_t* g_dst; // ссыль на растр от игры
NOT_EXPORTED static uint16_t g_w; // ширина растра
NOT_EXPORTED static uint16_t g_h; // высота растра
NOT_EXPORTED static int32_t g_value; // параметр яркости от эффекта

EXPORTED
void PLUG_CALL plugin_init(const struct context_t* context, struct result_t* result) {
  // описание плагина
  result->full_name = "Brightness";
  result->author = "(edit name)";
  result->description = "(edit description)";
  // проверка валидности данных
  if ( !check_params(context, result))
    return;
  // бинд параметров
  g_dst = context->dst;
  g_h = context->h;
  g_w = context->w;
  // создание строки в меню для управления параметром яркости
  g_value = 0;
  context->registrate_param_i32(
    "value",
    "brightness value",
    &g_value,
    4, -180, 180
  );
} // plugin_init

NOT_EXPORTED
int clamp(int x, int min, int max) {
  if (x < min)
    x = min;
  if (x > max)
    x = max;
  return x;
}

EXPORTED void PLUG_CALL plugin_apply(const uint32_t state) {
  // пройтись по всем пикселям
  for (size_t i = 0; i < g_w * g_h; ++i) {
    struct rgb24_t rgb = pal8_to_rgb24(g_dst[i]);
    rgb.r = clamp(rgb.r + g_value, 0, 255);
    rgb.g = clamp(rgb.g + g_value, 0, 255);
    rgb.b = clamp(rgb.b + g_value, 0, 255);
    g_dst[i] = rgb24_to_pal8(rgb);
  }
}

EXPORTED void PLUG_CALL plugin_finalize(void) {
  // сюда можно писать код для освобождения ресурсов выделенных в plugin_init
}
