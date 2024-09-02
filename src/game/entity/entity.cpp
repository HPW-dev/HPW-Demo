#include <cassert>
#include <utility>
#include "entity.hpp"
#include "util/error.hpp"
#include "graphic/effect/heat-distort.hpp"
#include "graphic/effect/light.hpp"
#include "game/core/canvas.hpp"
#include "game/core/graphic.hpp"
#include "game/core/debug.hpp"
#include "game/core/core.hpp"
#include "game/entity/entity-mgr.hpp"
#include "game/entity/util/entity-util.hpp"

Entity::Entity()
: Entity_cbs (*this)
, Entity_debug (*this)
, uid (get_entity_uid())
, type {GET_SELF_TYPE}
{ status.live = true; }

Entity::Entity(Entity_type new_type): Entity() { type = new_type; }

void Entity::kill() {
  status.live = false;
  status.killme = true;
  remove();
}

void Entity::remove() {
  status.live = false;
  status.removeme = true;
}

void Entity::process_kill() {
  status.killme = false;
  process_kill_cbs();
}

void Entity::process_remove() {
  status.removed = true;
  process_remove_cbs();
}

void Entity::draw(Image& dst, const Vec offset) const {
  if (!status.disable_render) {
    process_draw_bg_cbs(dst, offset);

    // отрисовка игрового объекта
    #ifdef DEBUG
    if (graphic::draw_entities)
    #endif
    {
      anim_ctx.draw(dst, *this, offset);
    }

    // вспышка
    if (light && graphic::enable_light && !status.disable_light)
      light->draw(dst, phys.get_pos() + offset);
      
    // искажение воздуха
    draw_haze(dst, offset);
    process_draw_fg_cbs(dst, offset);
  } // if !disable_render

  debug_draw(dst, offset);
}

// искажение воздуха
void Entity::draw_haze(Image& dst, const Vec offset) const {
  // не рисовать при лагах
  const bool auto_opt = graphic::render_lag && graphic::disable_heat_distort_while_lag;
  return_if(auto_opt);

  // если включено в настройках графики
  const bool graphic_enable = graphic::enable_heat_distort;
  const bool pointer_valid = scast<bool>(heat_distort);
  // если объектом не запрещено
  const bool entity_flag_enabled = !status.disable_heat_distort;
  
  if (graphic_enable && pointer_valid && entity_flag_enabled)
    heat_distort->draw(dst, phys.get_pos() + offset);
}

void Entity::update(const Delta_time dt) {
  if (!status.disable_motion)
    move_it(dt);
    
  anim_ctx.update(dt, *this);
  process_update_cbs(dt);

  if (heat_distort)
    heat_distort->update(dt);
  if (light)
    light->update(dt);
}

void Entity::set_master(Master_p new_master) {
  assert(new_master != this);
  master = new_master;
}

void Entity::move_it(const Delta_time dt) { phys.update(dt); }
void Entity::set_pos(const Vec pos) { phys.set_pos(pos); }
cp<Anim> Entity::get_anim() const { return anim_ctx.get_anim(); }
