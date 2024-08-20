#include "epge-autocontrast.hpp"
#include "plugin/graphic-effect/cxx/pge-util.hpp"

static Pal8* g_dst {}; // ссыль на растр от игры
static uint16_t g_w {}; // ширина растра
static uint16_t g_h {}; // высота растра

namespace Epge_autocontrast {

void plugin_init(cp<context_t> context, result_t* result) {
  // описание плагина
  result->full_name = "Auto-contrast";
  result->author = "HPW-dev";
  result->description = "Automaticly sets contrast value";
  // проверка валидности данных
  if ( !check_params(context, result))
    return;
  // бинд параметров
  static_assert(sizeof(pal8_t) == sizeof(Pal8));
  g_dst = ptr2ptr<Pal8*>(context->dst);
  g_h = context->h;
  g_w = context->w;
}

void plugin_apply(uint32_t state) {
  // TODO
}

void plugin_finalize() {}

} // epge ns
