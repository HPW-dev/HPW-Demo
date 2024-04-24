#include <cassert>
#include <algorithm>
#include "player-dark.hpp"
#include "game/util/keybits.hpp"
#include "game/core/core.hpp"
#include "game/core/canvas.hpp"
#include "game/core/common.hpp"
#include "game/core/entities.hpp"
#include "game/entity/util/scatter.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/util/info/anim-info.hpp"
#include "game/entity/util/info/collidable-info.hpp"
#include "util/hpw-util.hpp"
#include "util/file/yaml.hpp"
#include "util/math/random.hpp"
#include "util/math/vec-util.hpp"
#include "graphic/image/image.hpp"
#include "graphic/effect/light.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/animation/frame.hpp"

Player_dark::Player_dark(): Player() {}

/// спавнит мелкие пульки в след за мощным выстрелом
void spawn_small_bullets(Entity& master, double dt) {
  // TODO all vals by config

  // TODO by dt timer
  if ((hpw::game_updates_safe % 5) == 0) {
    auto it = hpw::entity_mgr->make(&master, "bullet.player.small",
      master.phys.get_pos());
    // замедлить эти пули
    it->phys.set_speed(it->phys.get_speed() * rndr(0.5, 1));
    // чтобы пули разлетались во все стороны
    it->phys.set_deg(it->phys.get_deg() + rndr(-45, 45));
    it->phys.set_force( 7.5_pps );
    it->move_update_callback( Kill_by_timeout(rndr(0.1, 0.7)) );
    it->status.layer_up = false;
  } // if timer
}

void Player_dark::shoot(double dt) {
  // стрелять менее часто, при нехватке энергии
  if (energy <= m_energy_level_for_decrease_shoot_speed)
    dt *= m_decrease_shoot_speed_ratio;

  // усиленый выстрел
  if (energy >= m_energy_for_power_shoot && check_capability(Ability::power_shoot)) {
    sub_en(m_power_shoot_price);
    power_shoot(dt);
  } else { // обычные выстрелы
    sub_en(m_shoot_price);
    default_shoot(dt);
  }
} // shoot

void Player_dark::power_shoot(double dt) {
  cfor (_, 30) { // TODO конфиг
    cauto spawn_pos = phys.get_pos() + Vec(rndr(-7, 7), 0); // TODO конфиг
    auto bullet = hpw::entity_mgr->make(this, "bullet.player.mid", spawn_pos);
    // пуля смотрит вверх в шмап моде
    bullet->phys.set_deg(270);
    // передача импульса для шмап мода
    bullet->phys.set_speed( pps(rndr(13, 17)) ); // TODO конфиг
    bullet->phys.set_vel(bullet->phys.get_vel() + phys.get_vel());
    // разброс
    bullet->phys.set_deg(bullet->phys.get_deg() + rndr(-75, 75)); // TODO конфиг
    bullet->move_update_callback(&spawn_small_bullets);
    bullet->status.layer_up = true;
  }

  // отдача
  hpw::entity_mgr->add_scatter(Scatter {
    .pos{ phys.get_pos() + Vec(0, -10) }, // создаёт источник взрыва перед ноосом
    .range{ 50 }, // TODO конфиг
    .power{ pps(60) }, // TODO конфиг
  });
} // power_shoot

void Player_dark::default_shoot(double dt) {
  cfor (_, m_shoot_timer.update(dt)) {
    cfor (bullet_count, m_default_shoot_count) { // несколько за раз
      cauto spawn_pos = phys.get_pos() + Vec(rndr(-7, 7), 0); // смещение пули при спавне
      auto bullet = hpw::entity_mgr->make(this, "bullet.player.small", spawn_pos);
      // пуля смотрит вверх в шмап моде
      bullet->phys.set_deg(270);
      bullet->phys.set_speed(m_shoot_speed);
      // передача импульса
      bullet->phys.set_vel(bullet->phys.get_vel() + phys.get_vel());
      // разброс
      auto deg_spread = is_pressed(hpw::keycode::focus)
        ? m_deg_focused_shoot
        : m_deg_spread_shoot;
      deg_spread = rndr(-deg_spread * 0.5, deg_spread * 0.5);
      bullet->phys.set_deg(bullet->phys.get_deg() + deg_spread);
    }
  } // for m_shoot_timer
} // default_shoot

void Player_dark::sub_en(hp_t val) { energy = std::max<hp_t>(0, energy - val); }
void Player_dark::energy_regen() { energy = std::min<hp_t>(energy_max, energy + m_energy_regen); }

void Player_dark::draw(Image& dst, const Vec offset) const {
  Collidable::draw(dst, offset);
  blink_contour();
  draw_stars(dst);
}

void Player_dark::update(double dt) {
  assert(hpw::shmup_mode); // вне шмап-мода этот класс не юзать

  energy_regen();
  set_dmg(get_hp()); /// урон при столкновении с игроком равен текущему хп игрока
  Player::update(dt);
  check_input(dt);
}

void Player_dark::check_input(double dt) {
  move(dt);

  // стрельба
  if (is_pressed(hpw::keycode::shoot))
    shoot(dt);
} // check_input

void Player_dark::move(double dt) {
  real spd = m_max_speed;
  phys.set_force(default_force);

  // фокусировка увеличивает сопротивление и уменьшает скорость
  if (is_pressed(hpw::keycode::focus)) {
    spd = m_focus_speed;
    phys.set_force(focus_force);
  }

  // определить направление движения игрока
  Vec dir;
  if ( is_pressed(hpw::keycode::up) && !is_pressed(hpw::keycode::down)) dir += Vec(0, -1);
  if (!is_pressed(hpw::keycode::up) &&  is_pressed(hpw::keycode::down)) dir += Vec(0, +1);
  if ( is_pressed(hpw::keycode::left) && !is_pressed(hpw::keycode::right)) dir += Vec(-1, 0);
  if (!is_pressed(hpw::keycode::left) &&  is_pressed(hpw::keycode::right)) dir += Vec(+1, 0);
  // сложение векторов
  if (dir) {
    auto motion = normalize_stable(dir) * spd;
    phys.set_vel(phys.get_vel() + motion);
    // не превышать скорость движения игрока
    phys.set_speed( std::min(phys.get_speed(), spd) );
  }

  // буст скорости в определённых направлениях
  if ( !is_pressed(hpw::keycode::focus)) {
    if (is_pressed(hpw::keycode::up) && !is_pressed(hpw::keycode::down)) {
      auto vel = phys.get_vel();
      vel.y *= m_boost_up;
      phys.set_vel(vel);
      phys.set_speed( std::min(phys.get_speed(), spd * m_boost_up) );
    }
    if (!is_pressed(hpw::keycode::up) && is_pressed(hpw::keycode::down)) {
      auto vel = phys.get_vel();
      vel.y *= m_boost_down;
      phys.set_vel(vel);
      phys.set_speed( std::min(phys.get_speed(), spd * m_boost_down) );
    }
  }
} // move

void Player_dark::blink_contour() const {
  assert(energy_max > 0);

  // конгда энергии мало, контур тусклый
  if (energy <= m_level_for_blink) {
    cauto ratio = energy / scast<real>(energy_max);
    anim_ctx.contour_bf = &blend_158;
    // чем меньше энергии, тем реже мерцать
    status.disable_contour = rndr_fast() >= ratio;
  } else { // конгда энергии много, контур белый и мерцает рандомно
    anim_ctx.contour_bf = &blend_past;
    status.disable_contour = rndb_fast() & 1;
  }
}

void Player_dark::draw_stars(Image& dst) const {
  assert(energy_max > 0);
  // расположение всех окошек бумера
  sconst Vector<Vec> window_pos_table {
                        {0, 0},
                   {-3, 2}, {3, 2},
               {-6, 5},         {6, 5},
           {-9, 8},                 {9, 8},
     {-12, 11},                         {12, 11},
  };

  cauto ratio = energy / scast<real>(energy_max);
  for (cnauto window_pos: window_pos_table) {
    // вспышка не всегда появляется
    cont_if (rndb_fast() > 16);

    // рисует звезду
    Light star;
    star.set_duration(1);
    assert(m_window_star_len > 0);
    star.radius = ratio * rndr_fast(0, m_window_star_len);
    star.flags.no_sphere = true;
    star.draw(dst, window_pos + anim_ctx.get_draw_pos());
  }
} // draw_stars

/// ограничитель позиции игрока в пределах экрана
struct Bound_off_screen {
  Vec screen_lu {}; /// ограничение слева сверху
  Vec screen_rd {}; /// ограничение справа снизу

  inline explicit Bound_off_screen(CN<Entity> src) {
    // получить размеры игрока
    auto anim = src.get_anim();
    assert(anim);
    auto frame = anim->get_frame(0);
    assert(frame);
    auto direct = frame->get_direct(0);
    assert(direct);
    auto sprite = direct->sprite.lock();
    assert(sprite);
    auto image = *sprite->get_image();
    Vec player_sz(image.X, image.Y);
    screen_lu = Vec(
      -1 * direct->offset.x,
      -1 * direct->offset.y
    );
    screen_rd = Vec(
      graphic::width  - player_sz.x - direct->offset.x,
      graphic::height - player_sz.y - direct->offset.y
    );
  } // c-tor

  inline void operator()(Entity& dst, double dt) {
    auto pos = dst.phys.get_pos();
    bool decrease_speed {false};
    if (pos.x < screen_lu.x)
      { pos.x = screen_lu.x; decrease_speed = true; }
    if (pos.x >= screen_rd.x)
      { pos.x = screen_rd.x-1; decrease_speed = true; }
    if (pos.y < screen_lu.y)
      { pos.y = screen_lu.y; decrease_speed = true; }
    if (pos.y >= screen_rd.y)
      { pos.y = screen_rd.y-1; decrease_speed = true; }
    // это фиксит быстрое движение при отталкивании
    if (decrease_speed)
      dst.phys.set_speed( dst.phys.get_speed() * 0.25 );
    dst.phys.set_pos(pos);
  } // op ()
}; // Bound_off_screen

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
  real m_percent_for_power_shoot {};
  real m_percent_for_power_shoot_price {};
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

  inline explicit Impl(CN<Yaml> config) {
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
    m_percent_for_power_shoot = shoot_node.get_real("percent_for_power_shoot");
    m_percent_for_power_shoot_price = shoot_node.get_real("percent_for_power_shoot_price");
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
    assert(m_percent_for_power_shoot > 0 && m_percent_for_power_shoot <= 100);
    assert(m_percent_for_power_shoot_price > 0 &&
      m_percent_for_power_shoot_price <= 100);
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
    
    nauto it = *scast<Player_dark*>(entity);
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
    it.m_energy_for_power_shoot = it.energy_max * (m_percent_for_power_shoot / 100.0);
    it.m_power_shoot_price = it.energy_max * (m_percent_for_power_shoot_price / 100.0);
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

Player_dark::Loader::Loader(CN<Yaml> config): impl{new_unique<Impl>(config)} {}
Player_dark::Loader::~Loader() {}
Entity* Player_dark::Loader::operator()(Entity* master, const Vec pos, Entity* parent) { return impl->operator()(master, pos, parent); }
