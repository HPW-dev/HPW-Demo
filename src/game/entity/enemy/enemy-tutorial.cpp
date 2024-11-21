#include <cassert>
#include "enemy-tutorial.hpp"
#include "util/hpw-util.hpp"
#include "util/file/yaml.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/random.hpp"
#include "game/core/common.hpp"
#include "game/core/entities.hpp"
#include "game/entity/player/player.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "game/entity/util/entity-util.hpp"

Enemy_tutorial::Enemy_tutorial(): Proto_enemy(GET_SELF_TYPE) {}

void Enemy_tutorial::update(const Delta_time dt) {
  assert(hpw::shmup_mode); // вне шмап-мода этот класс не юзать
  Proto_enemy::update(dt);
} // update

struct Enemy_tutorial::Loader::Impl {
  Info m_info {};

  inline explicit Impl(cr<Yaml> config) {
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
    assert(parent->type == ENTITY_TYPE(Enemy_tutorial));
    auto it = ptr2ptr<Enemy_tutorial*>(parent);
    it->m_info = m_info;
    //it->m_info.particle_timer.randomize_stable();
    return parent;
  } // op ()
}; // Impl

Enemy_tutorial::Loader::Loader(cr<Yaml> config)
: Proto_enemy::Loader(config)
, impl{new_unique<Impl>(config)}
{}

Entity* Enemy_tutorial::Loader::operator()(Entity* master, const Vec pos, Entity* parent) {
  auto ret = hpw::entity_mgr->allocate<Enemy_tutorial>();
  Proto_enemy::Loader::operator()(master, pos, ret);
  impl->operator()(master, pos, ret);
  return ret;
}

Enemy_tutorial::Loader::~Loader() {}
