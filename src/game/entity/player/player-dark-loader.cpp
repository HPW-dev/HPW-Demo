#include <cassert>
#include "player-dark.hpp"
#include "util/hpw-util.hpp"
#include "util/file/yaml.hpp"
#include "game/core/entities.hpp"
#include "game/entity/util/info/anim-info.hpp"
#include "game/entity/util/info/collidable-info.hpp"
#include "game/entity/util/entity-util.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/animation/anim.hpp"

struct Player_dark::Loader::Impl {
  Anim_info m_anim_info {};
  Collidable_info m_collidable_info {};
  real m_force {};
  real m_focus_force {};
  real m_max_speed {};
  real m_focus_speed {};
  real m_shoot_timer {};
  hp_t m_fuel {};
  hp_t m_shoot_price {};
  hp_t m_energy_regen {};
  hp_t m_energy_max {};
  real m_percent_for_decrease_shoot_speed {};
  real m_decrease_shoot_speed_ratio {};
  real m_deg_spread_shoot {};
  real m_deg_focused_shoot {};
  int m_default_shoot_count {};
  real m_shoot_speed {};
  real m_boost_up {};
  real m_boost_down {};
  real m_percent_level_for_blink {};
  real m_window_star_len {};

  inline explicit Impl(cr<Yaml> config) {
    m_collidable_info.load(config);

    cauto anim_node = config["animation"];
    m_anim_info.load(anim_node);
    assert(anim_node.check());
    m_percent_level_for_blink = anim_node.get_real("percent_level_for_blink");
    m_window_star_len         = anim_node.get_real("window_star_len");

    m_force       = config.get_real("force");
    m_focus_force = config.get_real("focus_force");
    m_max_speed   = config.get_real("max_speed");
    m_focus_speed = config.get_real("focus_speed");
    m_fuel        = config.get_int ("fuel");
    m_energy_max  = config.get_int ("energy_max");
    m_boost_up    = config.get_real("boost_up");
    m_boost_down  = config.get_real("boost_down");

    cauto shoot_node = config["shoot"];
    assert( shoot_node.check() );
    m_shoot_timer  = shoot_node.get_real("shoot_timer");
    m_shoot_price  = shoot_node.get_int ("shoot_price");
    m_energy_regen = shoot_node.get_int ("energy_regen");
    m_percent_for_decrease_shoot_speed = shoot_node.get_real("percent_for_decrease_shoot_speed");
    m_decrease_shoot_speed_ratio = shoot_node.get_real("decrease_shoot_speed_ratio");
    m_default_shoot_count = shoot_node.get_int("default_shoot_count");
    m_deg_spread_shoot = shoot_node.get_real("deg_spread_shoot");
    m_deg_focused_shoot = shoot_node.get_real("deg_focused_shoot");
    m_shoot_speed = shoot_node.get_real("shoot_speed");

    // проверка параметров
    assert(m_window_star_len > 0);
    assert(m_max_speed > 0);
    assert(m_focus_speed > 0);
    assert(m_shoot_timer > 0);
    assert(m_fuel > 0);
    assert(m_energy_regen > 0);
    assert(m_energy_max > 0);
    assert(m_percent_for_decrease_shoot_speed > 0 &&
      m_percent_for_decrease_shoot_speed < 100);
    assert(m_decrease_shoot_speed_ratio > 0);
    assert(m_default_shoot_count > 0);
    assert(m_shoot_speed > 0);
    assert(m_boost_up > 0);
    assert(m_boost_down > 0);
    assert(m_percent_level_for_blink > 0 && m_percent_level_for_blink < 100);
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    auto entity = hpw::entity_mgr->allocate<Player_dark>();
    Entity_loader::prepare(*entity, master, pos);
    m_collidable_info.accept(*entity);
    m_anim_info.accept(*entity);
    
    rauto it = *scast<Player_dark*>(entity);
    it.move_update_callback( Bound_off_screen(it) );
    it.phys.set_force( pps(m_force) );
    it.default_force = pps(m_force);
    it.focus_force = pps(m_focus_force);
    it.m_focus_speed = pps(m_focus_speed);
    it.m_max_speed = pps(m_max_speed);
    it.m_shoot_timer = m_shoot_timer;
    it.energy_max = m_energy_max;
    it.hp_max = it.get_hp();
    it.m_fuel = it.m_fuel_max = m_fuel;
    it.m_shoot_price = m_shoot_price;
    it.m_energy_regen = m_energy_regen;
    it.m_energy_level_for_decrease_shoot_speed = it.energy_max * (m_percent_for_decrease_shoot_speed / 100.0);
    it.m_decrease_shoot_speed_ratio = m_decrease_shoot_speed_ratio;
    it.m_default_shoot_count = m_default_shoot_count;
    it.m_deg_spread_shoot = m_deg_spread_shoot;
    it.m_deg_focused_shoot = m_deg_focused_shoot;
    it.m_shoot_speed = pps(m_shoot_speed);
    it.m_boost_up = m_boost_up;
    it.m_boost_down = m_boost_down;
    it.m_level_for_blink = it.energy_max * (m_percent_level_for_blink / 100.0);
    it.m_window_star_len = m_window_star_len;

    return entity;
  } // op ()
}; // Impl

Player_dark::Loader::Loader(cr<Yaml> config)
  : impl{new_unique<Impl>(config)} {}

Player_dark::Loader::~Loader() {}

Entity* Player_dark::Loader::operator()(Entity* master, const Vec pos, Entity* parent)
  { return impl->operator()(master, pos, parent); }
