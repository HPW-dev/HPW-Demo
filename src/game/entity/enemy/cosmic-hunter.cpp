#include <cassert>
#include "cosmic-hunter.hpp"
#include "util/file/yaml.hpp"
#include "util/hpw-util.hpp"
#include "graphic/image/image.hpp"
#include "game/game-common.hpp"
#include "game/entity/player.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/entity-manager.hpp"
#include "util/math/random.hpp"

Cosmic_hunter::Cosmic_hunter(): Proto_enemy(GET_SELF_TYPE) {}

void Cosmic_hunter::draw(Image& dst, const Vec offset) const {
  Proto_enemy::draw(dst, offset);
}

void Cosmic_hunter::update(double dt) {
  assert(hpw::shmup_mode); // вне шмап-мода этот класс не юзать
  cauto player = hpw::entity_mgr->get_player();
  return_if( !player);
  Proto_enemy::update(dt);
  
  // повернуться в игроку
  bool rot_to_right = need_rotate_right(*this, *player);
  phys.set_rot_spd(m_rotate_speed);
  phys.set_invert_rotation( !rot_to_right);

  cfor (_, shoot_timer.update(dt)) {
    // TODO conf
    auto bullet = hpw::entity_mgr->make(this, "bullet.cosmic.hunter", phys.get_pos());
    bullet->phys.set_speed(m_bullet_speed);
    bullet->status.ignore_scatter = true;

    real deg_to;
    switch (rndu(3)) {
      default:
      case 0: { // упреждение
        auto bullet_target = predict(*bullet, *player, dt);
        deg_to = deg_to_target(*bullet, bullet_target);
        break;
      }
      case 1: { // напрямую
        deg_to = deg_to_target(*bullet, *player);
        break;
      }
      case 2: { // напрямую с отклоениями
        deg_to = deg_to_target(*bullet, *player);
        deg_to += rndr(-m_shoot_deg, m_shoot_deg);
        break;
      }
    } // switch shoot type

    bullet->phys.set_deg(deg_to);
  } // for timer
} // update

struct Cosmic_hunter::Loader::Impl {
  real m_shoot_timer {};
  real m_speed {};
  real m_rotate_speed {};
  real m_bullet_speed {};
  real m_shoot_deg {};

  inline explicit Impl(CN<Yaml> config) {
    m_shoot_timer = config.get_real("shoot_timer");
    m_speed = config.get_real("speed");
    m_rotate_speed = config.get_real("rotate_speed");
    m_bullet_speed = config.get_real("bullet_speed");
    m_shoot_deg = config.get_real("shoot_deg");
    assert(m_bullet_speed > 0);
    assert(m_rotate_speed > 0);
    assert(m_speed > 0);
    assert(m_shoot_timer > 0);
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    assert(parent);
    assert(parent->type == ENTITY_TYPE(Cosmic_hunter));
    auto it = ptr2ptr<Cosmic_hunter*>(parent);

    it->phys.set_speed( pps(m_speed) );
    it->shoot_timer = Timer(m_shoot_timer);
    it->shoot_timer.randomize();
    it->m_rotate_speed = pps(m_rotate_speed);
    it->m_bullet_speed = pps(m_bullet_speed);
    it->m_shoot_deg = m_shoot_deg;
    // при спавне сразу смотрим в сторону игрока
    if (auto player = hpw::entity_mgr->get_player(); player)
      it->phys.set_deg( deg_to_target(*it, *player) );
    return parent;
  } // op ()

}; // Impl

Cosmic_hunter::Loader::Loader(CN<Yaml> config)
: Proto_enemy::Loader(config)
, impl{new_unique<Impl>(config)}
{}

Entity* Cosmic_hunter::Loader::operator()(Entity* master, const Vec pos, Entity* parent) {
  auto ret = hpw::entity_mgr->allocate<Cosmic_hunter>();
  Proto_enemy::Loader::operator()(master, pos, ret);
  impl->operator()(master, pos, ret);
  return ret;
}

Cosmic_hunter::Loader::~Loader() {}
