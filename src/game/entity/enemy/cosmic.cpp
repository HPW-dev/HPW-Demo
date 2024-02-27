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
#include "game/core/entities.hpp"
#include "game/core/canvas.hpp"
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
  Proto_enemy::draw(dst, offset);
}

void Cosmic::update(double dt) {
  assert(hpw::shmup_mode); // вне шмап-мода этот класс не юзать

  // останавливает первую анимацию после завершения
  status.no_restart_anim = m_fade_in_complete && !m_eyes_open_complete;

  Proto_enemy::update(dt);
  make_particles(dt);

  // появление
  if ( !m_fade_in_complete) {
    if (m_info.fade_in_timer.update(dt)) { // переход на следующую стадию
      m_fade_in_complete = true;
      // запустить рисунок
      anim_ctx.blend_f = &blend_diff;
      anim_ctx.set_speed_scale(1);
      status.disable_contour = false; // оставить контур включёным
    } else { // показывать контур
      anim_ctx.set_speed_scale(0); // не включать анимацию
      // мигать контуром
      status.disable_contour = rndr_fast() < m_info.fade_in_timer.ratio();
      anim_ctx.blend_f = &blend_none; // невидимый рисунок
    }
  }
  
  // время на анимацию открытия
  if (m_fade_in_complete && !m_eyes_open_complete) {
    if (m_info.eyes_open_timeout.update(dt)) {
      m_eyes_open_complete = true;
      anim_ctx.set_anim(m_info.state_2); // переход на финальную стадию
      anim_ctx.set_contour(m_info.contour);
      anim_ctx.contour_bf = &blend_past;
      anim_ctx.blend_f = &blend_max;
      status.disable_heat_distort = false;
    }
  }
  
  // основной алгоритм:
  return_if( !m_eyes_open_complete);

  // притяжение
  hpw::entity_mgr->add_scatter( Scatter {
    .pos {phys.get_pos()},
    .range {m_info.magnet_range},
    .power {m_info.magnet_power},
    .type {Scatter::Type::inside},
    .disable_shake {true},
  } );

  // стрелять в игрока
  cauto player = hpw::entity_mgr->get_player();
  assert(player);
  cfor (_, m_info.shoot_timer.update(dt)) {
    cfor (_, m_info.bullet_count) {
      cauto spawn_pos = phys.get_pos() + (rand_normalized_stable() *
        m_info.bullet_spawn_range);
      auto bullet = hpw::entity_mgr->make(this, m_info.bullet, spawn_pos);
      bullet->phys.set_speed(m_info.bullet_predict_speed);

      // либо стрелять в игрока, либо на упреждение
      real deg;
      if (rndb() & 1)
        deg = deg_to_target(*bullet, predict(*bullet, *player, dt) );
      else
        deg = deg_to_target(*bullet, *player);
      bullet->phys.set_deg(deg);

      bullet->phys.set_accel(m_info.bullet_accel);
      bullet->phys.set_speed(m_info.bullet_speed);
      bullet->status.ignore_scatter = true;
      bullet->status.layer_up = true;
    }
  }
} // update

void Cosmic::make_particles(double dt) {
  cauto w = graphic::width;
  cauto h = graphic::height;
  const int X = 12;
  const int Y = 9;
  cfor (_, m_info.particle_timer.update(dt)) {
    for (int y = -1; y < Y + 2; ++y)
    for (int x = -1; x < X + 2; ++x) {
      const Vec pos (
        (x + ((y & 1) ? 0 : 0.5)) * (w / X),
        y * (h / Y) );
      hpw::entity_mgr->make(this, "particle.blink.star", pos);
    } // for y x
  } // for particle_timer
}

struct Cosmic::Loader::Impl {
  Info m_info {};

  inline explicit Impl(CN<Yaml> config) {
    cauto animations = config.get_v_str("animations");
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
    assert( !m_info.bullet.empty());
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    assert(parent);
    assert(parent->type == ENTITY_TYPE(Cosmic));
    auto it = ptr2ptr<Cosmic*>(parent);
    it->m_info = m_info;
    // первая анимация проигрывается до конца и не перезапускается
    it->status.no_restart_anim = true;
    it->status.layer_up = true;
    it->anim_ctx.set_anim(m_info.state_1);
    it->anim_ctx.set_contour(m_info.contour);
    it->anim_ctx.contour_bf = &blend_past;
    it->anim_ctx.blend_f = &blend_max;
    it->heat_distort = new_shared<Heat_distort>(m_info.heat_distort);
    it->status.disable_heat_distort = true;
    it->m_info.shoot_timer.randomize();
    it->m_info.particle_timer.randomize();
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
