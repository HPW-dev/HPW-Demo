#include <cassert>
#include <utility>
#include "entity.hpp"
#include "util/error.hpp"
#include "graphic/effect/heat-distort.hpp"
#include "graphic/effect/light.hpp"
#include "graphic/font/font.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/game-font.hpp"
#include "game/game-canvas.hpp"
#include "game/game-debug.hpp"
#include "game/game-common.hpp"
#include "game/game-graphic.hpp"
#include "game/game-core.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/util/entity-util.hpp"

Entity::Entity()
: update_callbacks {}
, phys {}
, anim_ctx {}
, heat_distort {}
, light {}
, master {}
, uid (get_entity_uid())
, status {}
, type {GET_SELF_TYPE}
{ status.live = true; }

Entity::Entity(Entity_type new_type): Entity() { type = new_type; }

void Entity::kill() { status.live = false; }

void Entity::draw(Image& dst, const Vec offset) const {
  // отрисовка игрового объекта
  if (graphic::draw_entitys)
    anim_ctx.draw(dst, *this, offset);

  // вспышка
  if (light && graphic::enable_light && !status.disable_light)
    light->draw(dst, phys.get_pos() + offset);
    
  // искажение воздуха
  if (
    graphic::enable_heat_distort &&
    heat_distort &&
    !status.disable_heat_distort &&
    !(graphic::render_lag && graphic::disable_heat_distort_while_lag)
  ) {
    heat_distort->draw(dst, phys.get_pos() + offset);
  }
  
  debug_draw(dst, offset);
} // draw

void Entity::update(double dt) {
  move_it(dt);
  anim_ctx.update(dt, *this);

  // применить внешние колбэки
  for (cnauto callback: update_callbacks)
    callback(*this, dt);

  if (heat_distort && graphic::enable_heat_distort && !status.disable_heat_distort)
    heat_distort->update(dt);
  if (light && graphic::enable_light && !status.disable_light)
    light->update(dt);
}

void Entity::set_master(Master_p new_master) {
  assert(new_master != this);
  master = new_master;
}

void Entity::move_it(double dt) {
  phys.update(dt); // тут оказались
}

void Entity::set_pos(const Vec pos) {
  phys.set_pos(pos);
}

CP<Anim> Entity::get_anim() const {
  return anim_ctx.get_anim();
}

void Entity::draw_pos(Image& dst, const Vec offset) const {
  auto pos = phys.get_pos();
  utf32 pos_txt =
    U"(" +
    n2s<utf32>(pos.x, 1) +
    U", " +
    n2s<utf32>(pos.y, 1) +
    U")";
  graphic::font->draw(dst, pos + offset + Vec(5, 5), pos_txt, &blend_diff);
}

void Entity::debug_draw(Image& dst, const Vec offset) const {
  if (graphic::draw_entity_pos) {
    // отрисовка позиций объекта
    draw_pos(dst, offset);
    // перекрестие по центру
    draw_line<&blend_diff>(dst, Vec(0, graphic::height/2.0), Vec(graphic::width, graphic::height/2.0), Pal8::white);
    draw_line<&blend_diff>(dst, Vec(graphic::width/2.0, 0), Vec(graphic::width/2.0, graphic::height), Pal8::white);
  }
}

void Entity::move_update_callback(Update_callback&& callback) {
  if (callback)
    update_callbacks.emplace_back(std::move(callback));
}

void Entity::clear_callbacks() { update_callbacks.clear(); }
