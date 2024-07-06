#include <cassert>
#include "collidable.hpp"
#include "util/hitbox.hpp"
#include "util/phys.hpp"
#include "util/error.hpp"
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
  Entity::update(dt);

  // когда заканчивается обработка столкновений, флаг выключается
  status.collided = false;
  m_collided.clear();
}

bool Collidable::is_collided_with(CN<Collidable> other) const {
  // столкновение с собой не проверять
  if (this == std::addressof(other))
    return false;

  auto this_hitbox = this->get_hitbox();
  if ( !this_hitbox)
    return false;
  cauto this_pos = phys.get_pos();
  
  auto other_hitbox = other.get_hitbox();
  if ( !other_hitbox)
    return false;
  cauto other_pos = other.phys.get_pos();

  return this_hitbox->is_collided_with(this_pos, other_pos, *other_hitbox);
} // is_collided_with

CP<Hitbox> Collidable::get_hitbox() const
  { return anim_ctx_util::get_hitbox(anim_ctx, phys.get_deg(), *this); }

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
  if (get_hp() <= 0)
    status.killed = true;
}

void Collidable::kill() {
  Entity::kill();
  // запустить анимацию взрыва, если она есть
  if ( !m_explosion_name.empty())
    hpw::entity_mgr->make(this, m_explosion_name, phys.get_pos());
}

void Collidable::collide(Collidable& other) {
  // не сталкиваться с объектом, если уже столкнулись
  return_if(this->collided_with(std::addressof(other)));

  // нанести урон
  this->status.collided = true;
  this->sub_hp( other.get_dmg() );
  other.status.collided = true;
  other.sub_hp( this->get_dmg() );

  // вписать инфу о том, с кем столкнулись
  this->m_collided.emplace(std::addressof(other));
  other.m_collided.emplace(this);
}
