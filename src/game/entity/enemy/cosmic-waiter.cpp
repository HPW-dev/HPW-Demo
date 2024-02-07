#include <cassert>
#include "cosmic-waiter.hpp"
#include "util/file/yaml.hpp"
#include "util/hpw-util.hpp"
#include "graphic/image/image.hpp"
#include "game/game-common.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/entity-manager.hpp"

Cosmic_waiter::Cosmic_waiter(): Proto_enemy(GET_SELF_TYPE) {}

void Cosmic_waiter::draw(Image& dst, const Vec offset) const {
  Proto_enemy::draw(dst, offset);
}

void Cosmic_waiter::update(double dt) {
  assert(hpw::shmup_mode); // вне шмап-мода этот класс не юзать
  Proto_enemy::update(dt);
} // update

struct Cosmic_waiter::Loader::Impl {
  real m_start_speed {};
  real m_accel {};

  inline explicit Impl(CN<Yaml> config) {
    m_start_speed = config.get_real("start_speed");
    m_accel = config.get_real("accel");
    assert(m_start_speed > 0);
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    assert(parent);
    assert(parent->type == ENTITY_TYPE(Cosmic_waiter));
    auto it = ptr2ptr<Cosmic_waiter*>(parent);

    it->phys.set_deg(90); // смотреть вниз и лететь туда с ускорением
    it->phys.set_speed( pps(m_start_speed) );
    it->phys.set_accel( pps(m_accel) );
    it->anim_ctx.set_speed_scale(0); // TODO костыль выключающий анимаю

    return parent;
  } // op ()

}; // Impl

Cosmic_waiter::Loader::Loader(CN<Yaml> config)
: Proto_enemy::Loader(config)
, impl{new_unique<Impl>(config)}
{}

Entity* Cosmic_waiter::Loader::operator()(Entity* master, const Vec pos, Entity* parent) {
  auto ret = hpw::entity_mgr->allocate<Cosmic_waiter>();
  Proto_enemy::Loader::operator()(master, pos, ret);
  impl->operator()(master, pos, ret);
  return ret;
}

Cosmic_waiter::Loader::~Loader() {}
