#include "entity-animated.hpp"
#include "entity.hpp"
#include "graphic/effect/heat-distort.hpp"
#include "graphic/effect/light.hpp"
#include "game/core/canvas.hpp"
#include "game/core/graphic.hpp"
#include "game/core/debug.hpp"

// искажение воздуха
void Entity_animated::draw_haze(Image& dst, const Vec offset) const {
  // не рисовать при лагах
  const bool auto_opt = graphic::render_lag && graphic::disable_heat_distort_while_lag;
  return_if(auto_opt);

  // если включено в настройках графики
  const bool graphic_enable = graphic::enable_heat_distort;
  const bool pointer_valid = scast<bool>(heat_distort);
  // если объектом не запрещено
  const bool entity_flag_enabled = !_master.status.disable_heat_distort;
  
  if (graphic_enable && pointer_valid && entity_flag_enabled)
    heat_distort->draw(dst, _master.phys.get_pos() + offset);
}

cp<Anim> Entity_animated::get_anim() const { return anim_ctx.get_anim(); }

void Entity_animated::draw(Image& dst, const Vec offset) const {
  if (!_master.status.disable_render) {
    _master.process_draw_bg_cbs(dst, offset);

    // отрисовка игрового объекта
    #ifdef DEBUG
    if (graphic::draw_entities)
    #endif
    {
      anim_ctx.draw(dst, _master, offset);
    }

    // вспышка
    const bool VALID_LIGHT_PTR = scast<bool>(light);
    const bool LIGHT_NOT_DISABLED = graphic::light_quality != Light_quality::disabled;
    const bool LIGHT_NOT_DISABLED_IN_MASTER = !_master.status.disable_light;
    if (VALID_LIGHT_PTR && LIGHT_NOT_DISABLED && LIGHT_NOT_DISABLED_IN_MASTER)
      light->draw(dst, _master.phys.get_pos() + offset);
      
    // искажение воздуха
    draw_haze(dst, offset);
    _master.process_draw_fg_cbs(dst, offset);
  } // if !disable_render

  debug_draw(dst, offset);
}

void Entity_animated::update(const Delta_time dt) {
  anim_ctx.update(dt, _master);
  if (heat_distort)
    heat_distort->update(dt);
  if (light)
    light->update(dt);
}
