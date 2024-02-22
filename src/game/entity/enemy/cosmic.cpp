#include <cassert>
#include "cosmic.hpp"
#include "util/file/yaml.hpp"
#include "util/hpw-util.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/random.hpp"
#include "graphic/image/image.hpp"
#include "graphic/animation/animation-manager.hpp"
#include "graphic/animation/anim.hpp"
#include "game/core/common.hpp"
#include "game/core/entitys.hpp"
#include "game/entity/player.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "game/entity/util/scatter.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/util/game-util.hpp"
#include "graphic/effect/heat-distort.hpp"
#include "graphic/effect/heat-distort-util.hpp"

Cosmic::Cosmic(): Proto_enemy(GET_SELF_TYPE) {}

void Cosmic::draw(Image& dst, const Vec offset) const {
  // TODO костыль, останавливает анимацию после завершения
  if (m_fade_in_complete && !m_eyes_open_complete) {
    if (status.end_anim)
      anim_ctx.set_last_frame();
  }

  Proto_enemy::draw(dst, offset);
}

void Cosmic::update(double dt) {
  assert(hpw::shmup_mode); // вне шмап-мода этот класс не юзать
  Proto_enemy::update(dt);

  // появление
  if ( !m_fade_in_complete) {
    if (m_fade_in_timer.update(dt)) { // переход на следующую стадию
      m_fade_in_complete = true;
      // запустить рисунок и выключить контур
      anim_ctx.blend_f = &blend_diff;
      anim_ctx.set_speed_scale(1);
      status.disable_contour = true;
    } else { // показывать контур
      anim_ctx.set_speed_scale(0); // не включать анимацию
      // мигать контуром
      status.disable_contour = rndr_fast() < m_fade_in_timer.ratio();
      anim_ctx.blend_f = &blend_none; // невидимый рисунок
      anim_ctx.contour_bf = &blend_158;
    }
  }
  
  // время на анимацию открытия
  if (m_fade_in_complete && !m_eyes_open_complete) {
    if (m_eyes_open_timeout.update(dt)) {
      m_eyes_open_complete = true;
      anim_ctx.set_anim(m_state_2); // переход на финальную стадию
      status.disable_heat_distort = false;
      status.layer_up = false;
    }
  }
  
  // основной алгоритм:
  return_if( !m_eyes_open_complete);
  hpw::entity_mgr->add_scatter( Scatter {
    .pos {phys.get_pos()},
    .range {m_magnet_range},
    .power {m_magnet_power},
    .type {Scatter::Type::inside},
    .disable_shake {true},
  } );

  // стрелять в игрока
  cauto player = hpw::entity_mgr->get_player();
  assert(player);
  cfor (_, m_shoot_timer.update(dt)) {
    cfor (_, 2) {
      // TODO conf
      cauto spawn_pos = phys.get_pos() + (rand_normalized_stable() * m_bullet_spawn_range);
      auto bullet = hpw::entity_mgr->make(this, m_bullet, spawn_pos);
      bullet->phys.set_speed(7_pps); // скорость для предикта

      // либо стрелять в игрока, либо на упреждение
      real deg;
      if (rndb() & 1)
        deg = deg_to_target(*bullet, predict(*bullet, *player, dt) );
      else
        deg = deg_to_target(*bullet, *player);
      bullet->phys.set_deg(deg);

      bullet->phys.set_accel(m_bullet_accel);
      bullet->phys.set_speed(m_bullet_speed);
      bullet->status.ignore_scatter = true;
      bullet->status.layer_up = true;
    }
  }
} // update

struct Cosmic::Loader::Impl {
  Anim* m_state_1 {}; /// анимация на стадии появления
  Anim* m_state_2 {}; /// основная анимация с хитбоксом
  Anim* m_contour {};
  real m_fade_in_time {}; /// время на появление из темноты
  real m_eyes_open_timeout {}; /// время на анимацию открытия глаз
  real m_magnet_range {};
  real m_magnet_power {};
  real m_shoot_timer {};
  real m_bullet_spawn_range {};
  real m_bullet_speed {};
  real m_bullet_accel {};
  Str m_bullet {};
  Heat_distort m_heat_distort {};

  inline explicit Impl(CN<Yaml> config) {
    cauto animations = config.get_v_str("animations");
    m_state_1 = hpw::anim_mgr->find_anim(animations.at(0)).get();
    m_state_2 = hpw::anim_mgr->find_anim(animations.at(1)).get();
    m_contour = make_light_mask(m_state_1->get_name(),
      m_state_1->get_name() + ".light_mask").get();

    m_fade_in_time = config.get_real("fade_in_time");
    m_eyes_open_timeout = config.get_real("eyes_open_timeout");
    m_magnet_range = config.get_real("magnet_range");
    m_magnet_power = config.get_real("magnet_power");
    m_shoot_timer = config.get_real("shoot_timer");
    m_bullet_spawn_range = config.get_real("bullet_spawn_range");
    m_bullet_speed = config.get_real("bullet_speed");
    m_bullet_accel = config.get_real("bullet_accel");
    m_bullet = config.get_str("bullet");
    m_heat_distort = load_heat_distort(config["heat_distort"]);

    assert(m_state_1);
    assert(m_fade_in_time > 0);
    assert(m_eyes_open_timeout > 0);
    assert(m_magnet_range > 0);
    assert(m_magnet_power > 0);
    assert(m_shoot_timer > 0);
    assert(m_bullet_speed > 0);
    assert( !m_bullet.empty());
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    assert(parent);
    assert(parent->type == ENTITY_TYPE(Cosmic));
    auto it = ptr2ptr<Cosmic*>(parent);

    it->anim_ctx.set_anim(m_state_1);
    it->anim_ctx.set_contour(m_contour);
    it->anim_ctx.contour_bf = &blend_past;
    it->heat_distort = new_shared<Heat_distort>(m_heat_distort);
    it->status.disable_heat_distort = true;
    it->m_state_1 = m_state_1;
    it->m_state_2 = m_state_2;
    it->m_fade_in_timer = Timer(m_fade_in_time);
    it->m_eyes_open_timeout = Timer(m_eyes_open_timeout);
    it->m_shoot_timer = Timer(m_shoot_timer);
    it->m_shoot_timer.randomize();
    it->m_magnet_range = m_magnet_range;
    it->m_magnet_power = pps(m_magnet_power);
    it->m_bullet_spawn_range = m_bullet_spawn_range;
    it->m_bullet_speed = pps(m_bullet_speed);
    it->m_bullet = m_bullet;
    it->m_bullet_accel = pps(m_bullet_accel);

    return parent;
  } // op ()

}; // Impl

Cosmic::Loader::Loader(CN<Yaml> config)
: Proto_enemy::Loader(config)
, impl{new_unique<Impl>(config)}
{}

Entity* Cosmic::Loader::operator()(Entity* master, const Vec pos, Entity* parent) {
  auto ret = hpw::entity_mgr->allocate<Cosmic>();
  Proto_enemy::Loader::operator()(master, pos, ret);
  impl->operator()(master, pos, ret);
  return ret;
}

Cosmic::Loader::~Loader() {}
