#include <cassert>
#include "util/phys.hpp"
#include "util/error.hpp"
#include "collidable.hpp"
#include "util/hitbox.hpp"
#include "util/entity-util.hpp"
#include "util/anim-ctx-util.hpp"
#include "game/core/debug.hpp"
#include "game/core/entities.hpp"
#include "graphic/image/image.hpp"

Collidable::Collidable()
: Entity(GET_SELF_TYPE)
, m_collided {}
, m_hp {}
, m_dmg {}
, m_explosion_name {}
{ status.collidable = true; }

Collidable::Collidable(Entity_type new_type): Collidable()
{ type = new_type; }

void Collidable::draw(Image& dst, const Vec offset) const {
  Entity::draw(dst, offset);
  
  // показать хитбокс, если надо
  if (graphic::draw_hitboxes) 
    draw_hitbox(dst, offset);
}

void Collidable::update(const Delta_time dt) {
  process_damage();
  return_if (kill_by_damage());
  Entity::update(dt);

  // когда заканчивается обработка столкновений, флаг выключается
  status.collided = false;
  m_collided.clear();
}

void Collidable::process_damage() {
  for (cnauto other: m_collided)
    sub_hp( other->get_dmg() );
}

bool Collidable::kill_by_damage() {
  return_if (get_hp() > 0, false);
  status.killed = true;
  return true;
}

bool Collidable::hitbox_test(CN<Collidable> other) const {
  // столкновение с собой не проверять
  return_if (this == std::addressof(other), false);

  auto this_hitbox = this->get_hitbox();
  return_if (!this_hitbox, false);
  cauto this_pos = phys.get_pos();
  
  auto other_hitbox = other.get_hitbox();
  return_if (!other_hitbox, false);
  cauto other_pos = other.phys.get_pos();

  return this_hitbox->is_collided_with(this_pos, other_pos, *other_hitbox);
} // hitbox_test

CP<Hitbox> Collidable::get_hitbox() const {
  cauto deg = phys.get_deg();

  // если угол не менялся, то вернуть кешированный результат
  const bool deg_is_equal = m_old_deg == deg;
  const bool anim_is_equal = m_old_anim == anim_ctx.get_anim();
  if (deg_is_equal && anim_is_equal)
    return m_old_hitbox;

  // кэшировать новый результат
  auto ret = anim_ctx_util::get_hitbox(anim_ctx, deg, *this);
  m_old_deg = deg;
  m_old_hitbox = ret;
  m_old_anim = anim_ctx.get_anim();
  return ret;
}

void Collidable::draw_hitbox(Image& dst, const Vec offset) const {
  if (auto local_hitbox = get_hitbox(); local_hitbox) {
    Pal8 color = this->status.collided ? Pal8::red : Pal8::white;
    local_hitbox->draw(phys.get_pos() + offset, *hpw::hitbox_layer, color);
  }
}

void Collidable::sub_hp(hp_t incoming_dmg) {
  static_assert(std::is_signed<decltype(m_hp)>()); // иначе переделай момент с выходом за 0
  // вычесть дамаг, если кончились жизни - сдохнуть
  set_hp( get_hp() - incoming_dmg );
}

void Collidable::kill() {
  Entity::kill();
  // запустить анимацию взрыва, если она есть
  if ( !m_explosion_name.empty())
    hpw::entity_mgr->make(this, m_explosion_name, phys.get_pos());
}

bool Collidable::collision_possible(Collidable& other) const {
  // с собой не врезаться
  return_if (this == std::addressof(other), false);
  // надо быть живым
  return_if (!status.live || !other.status.live, false);
  // не сталкиваться с объектом, если уже столкнулись
  return_if (this->is_collided_with(std::addressof(other)), false);
  // проверить что по флагам можно сталкиваться
  return_if (!cld_flag_compat(*this, other), false);
  // всё гуд, можно проверять столкновения детальнее
  return true;
}

void Collidable::collide_with(Collidable& other) {
  omp::lock_guard lock_this(this->m_mutex);
  omp::lock_guard lock_other(other.m_mutex);
  this->status.collided = true;
  other.status.collided = true;
  // вписать инфу о том, с кем столкнулись
  this->m_collided.emplace(std::addressof(other));
  other.m_collided.emplace(this);
}

bool Collidable::resolve_collision(Collidable& other) {
  return_if (!collision_possible(other), false);
  if (hitbox_test(other))
    collide_with(other);
  return false;
}

bool Collidable::is_collided_with(Collidable* other) const {
  assert(other);
  omp::lock_guard lock(m_mutex);
  return m_collided.contains(other);
}
