#include <cassert>
#include "illaen.hpp"
#include "util/hpw-util.hpp"
#include "util/file/yaml.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/random.hpp"
#include "graphic/image/image.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/effect/heat-distort.hpp"
#include "graphic/effect/heat-distort-util.hpp"
#include "game/core/common.hpp"
#include "game/core/entities.hpp"
#include "game/core/anims.hpp"
#include "game/core/canvas.hpp"
#include "game/util/game-util.hpp"
#include "game/entity/player.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "game/entity/util/scatter.hpp"
#include "game/entity/util/entity-util.hpp"

Illaen::Illaen(): Proto_enemy(GET_SELF_TYPE) {}

void Illaen::draw(Image& dst, const Vec offset) const {
  Proto_enemy::draw(dst, offset);
}

void Illaen::update(const Delta_time dt) {
  assert(hpw::shmup_mode); // вне шмап-мода этот класс не юзать
  Proto_enemy::update(dt);
  
  switch (m_info.state) {
    case State::fade_in: fade_in(dt); break;
    default:
    case State::attack: attack(dt); break;
    case State::fade_out: fade_out(dt); break;
    case State::teleport: teleport(dt); break;
  }
} // update

void Illaen::fade_in(const Delta_time dt) {
  // TODO
}

void Illaen::attack(const Delta_time dt) {
  // TODO
}

void Illaen::fade_out(const Delta_time dt) {
  // TODO
}

void Illaen::teleport(const Delta_time dt) {
  // TODO
}

void Illaen::update_magnet(const Delta_time dt) {
  // TODO
}

void Illaen::make_particles(const Delta_time dt) {
  // TODO
}

struct Illaen::Loader::Impl {
  Info m_info {};

  inline explicit Impl(CN<Yaml> config) {
    /*cauto animations = config.get_v_str("animations");
    m_info.state_1 = hpw::anim_mgr->find_anim(animations.at(0)).get();
    m_info.state_2 = hpw::anim_mgr->find_anim(animations.at(1)).get();
    m_info.contour = make_light_mask(m_info.state_1->get_name(),
      m_info.state_1->get_name() + ".light_mask").get();
    m_info.eyes_open_timeout = Timer( config.get_real("eyes_open_timeout") );
    m_info.fade_in_timer = Timer( config.get_real("fade_in_time") );
    m_info.shoot_timer = Timer( config.get_real("shoot_timer") );
    m_info.particle_timer = Timer( config.get_real("particle_timer") );
    m_info.magnet_range = config.get_real("magnet_range");
    m_info.magnet_power = pps( config.get_real("magnet_power") );
    m_info.bullet_spawn_range = config.get_real("bullet_spawn_range");
    m_info.bullet_speed = pps( config.get_real("bullet_speed") );
    m_info.bullet_accel = pps( config.get_real("bullet_accel") );
    m_info.bullet_predict_speed = pps( config.get_real("bullet_predict_speed") );
    m_info.bullet = config.get_str("bullet");
    m_info.heat_distort = load_heat_distort(config["heat_distort"]);
    m_info.bullet_count = config.get_int("bullet_count");

    assert(m_info.state_1);
    assert(m_info.state_2);
    assert(m_info.bullet_count > 0);
    assert(m_info.magnet_range > 0);
    assert(m_info.magnet_power > 0);
    assert(m_info.bullet_speed > 0);
    assert(m_info.bullet_predict_speed > 0);
    assert( !m_info.bullet.empty());*/
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    assert(parent);
    assert(parent->type == ENTITY_TYPE(Illaen));
    auto it = ptr2ptr<Illaen*>(parent);
    it->m_info = m_info;
    //it->m_info.particle_timer.randomize_stable();
    return parent;
  } // op ()
}; // Impl

Illaen::Loader::Loader(CN<Yaml> config)
: Proto_enemy::Loader(config)
, impl{new_unique<Impl>(config)}
{}

Entity* Illaen::Loader::operator()(Entity* master, const Vec pos, Entity* parent) {
  auto ret = hpw::entity_mgr->allocate<Illaen>();
  Proto_enemy::Loader::operator()(master, pos, ret);
  impl->operator()(master, pos, ret);
  return ret;
}

Illaen::Loader::~Loader() {}
