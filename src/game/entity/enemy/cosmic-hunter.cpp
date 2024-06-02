#include <cassert>
#include "cosmic-hunter.hpp"
#include "util/file/yaml.hpp"
#include "util/hpw-util.hpp"
#include "graphic/image/image.hpp"
#include "game/core/common.hpp"
#include "game/core/entities.hpp"
#include "game/entity/player.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/entity-manager.hpp"
#include "util/math/random.hpp"

Cosmic_hunter::Cosmic_hunter(): Proto_enemy(GET_SELF_TYPE) {}

void Cosmic_hunter::draw(Image& dst, const Vec offset) const {
  Proto_enemy::draw(dst, offset);
}

void Cosmic_hunter::update(const Delta_time dt) {
  assert(hpw::shmup_mode); // вне шмап-мода этот класс не юзать
  cauto player = hpw::entity_mgr->get_player();
  return_if( !player);
  Proto_enemy::update(dt);
  
  // повернуться в игроку
  bool rot_to_right = need_rotate_right(*this, *player);
  phys.set_rot_spd(m_info.rotate_speed);
  phys.set_invert_rotation( !rot_to_right);

  cfor (_, m_info.shoot_timer.update(dt)) {
    auto bullet = hpw::entity_mgr->make(this, m_info.bullet_name, phys.get_pos());
    bullet->phys.set_speed(m_info.bullet_speed);
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
        deg_to += rndr(-m_info.shoot_deg, m_info.shoot_deg);
        break;
      }
    } // switch shoot type

    bullet->phys.set_deg(deg_to);
  } // for timer
} // update

struct Cosmic_hunter::Loader::Impl {
  Info m_info {};

  inline explicit Impl(CN<Yaml> config) {
    m_info.shoot_timer = Timer( config.get_real("shoot_timer") );
    m_info.speed = pps( config.get_real("speed") );
    m_info.rotate_speed = pps( config.get_real("rotate_speed") );
    m_info.bullet_speed = pps( config.get_real("bullet_speed") );
    m_info.shoot_deg = config.get_real("shoot_deg");
    m_info.bullet_name = config.get_str("bullet_name");
    
    assert(m_info.bullet_speed > 0);
    assert(m_info.rotate_speed > 0);
    assert(m_info.speed > 0);
    assert( !m_info.bullet_name.empty());
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    assert(parent);
    assert(parent->type == ENTITY_TYPE(Cosmic_hunter));
    auto it = ptr2ptr<Cosmic_hunter*>(parent);
    it->m_info = m_info;
    it->m_info.shoot_timer.randomize_stable();
    it->phys.set_speed(m_info.speed);
    // при спавне сразу смотрим в сторону игрока
    it->phys.set_deg( deg_to_target(*it, hpw::entity_mgr->target_for_enemy()) );
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
