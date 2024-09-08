#include <cassert>
#include <algorithm>
#include "player-dark.hpp"
#include "game/util/keybits.hpp"
#include "game/core/core.hpp"
#include "game/core/canvas.hpp"
#include "game/core/common.hpp"
#include "game/core/entities.hpp"
#include "util/math/random.hpp"
#include "util/math/vec-util.hpp"
#include "util/log.hpp"
#include "graphic/image/image.hpp"
#include "graphic/effect/light.hpp"

Player_dark::Player_dark(): Player() {}

void Player_dark::shoot(const Delta_time dt) {
  // attack_cooldown_amp изменит задержку выстрела 
  default_shoot(dt * attack_cooldown_amp);
}

void Player_dark::default_shoot(const Delta_time dt) {
  // подождать кулдаун
  cfor (_, m_shoot_timer.update(dt)) {
    // пустить несколько пуль за раз
    cfor (bullet_count, m_default_shoot_count) {
      cont_if(energy < m_shoot_price);
      sub_en(m_shoot_price);
      // смещение пули при спавне
      cauto spawn_pos = phys.get_pos() + Vec(rndr(-7, 7), 0);
      auto bullet = hpw::entity_mgr->make(this,
        "bullet.player.small", spawn_pos);
      // пуля смотрит вверх в шмап моде
      bullet->phys.set_deg(270);
      bullet->phys.set_speed(m_shoot_speed);
      // передача импульса
      bullet->phys.set_vel((bullet->phys.get_vel() + phys.get_vel()) * bullet_speed_amp);
      // разброс
      auto deg_spread = is_pressed(hpw::keycode::focus)
        ? m_deg_focused_shoot
        : m_deg_spread_shoot;
      deg_spread = rndr(-deg_spread * 0.5, deg_spread * 0.5);
      bullet->phys.set_deg(bullet->phys.get_deg() + deg_spread);
    }
  } // for m_shoot_timer
} // default_shoot

void Player_dark::energy_regen() { energy = std::min<hp_t>(energy_max, energy + m_energy_regen); }

void Player_dark::draw(Image& dst, const Vec offset) const {
  return_if(status.disable_render);
  Player::draw(dst, offset);
  blink_contour();
  draw_stars(dst);
}

void Player_dark::update(const Delta_time dt) {
  assert(hpw::shmup_mode); // вне шмап-мода этот класс не юзать

  energy_regen();
  set_dmg(get_hp()); // урон при столкновении с игроком равен текущему хп игрока
  Player::update(dt);
  check_input(dt);
}

void Player_dark::check_input(const Delta_time dt) {
  process_motion();

  // стрельба
  if (is_pressed(hpw::keycode::shoot))
    shoot(dt);
} // check_input

void Player_dark::process_motion() {
  cauto spd = process_speed();
  cauto dir = process_motion_dir();

  // инерция через сложение векторов
  if (dir.not_zero()) {
    cauto motion = normalize_stable(dir) * spd;
    phys.set_vel(phys.get_vel() + motion);
    // не превышать скорость движения игрока
    cauto new_speed = std::min(phys.get_speed(), spd);
    phys.set_speed(new_speed);
  }

  accept_speed_boost(spd);
} // move

Vec Player_dark::process_motion_dir() const {
  Vec dir;
  // Игнорить одновременно зажатые вправ+лево и т.д.
  if (pressed_up())    dir += Vec(0, -1);
  if (pressed_down())  dir += Vec(0, +1);
  if (pressed_left())  dir += Vec(-1, 0);
  if (pressed_right()) dir += Vec(+1, 0);
  return dir;
}

real Player_dark::process_speed() {
  real spd = m_max_speed;
  phys.set_force(default_force);

  // фокусировка увеличивает сопротивление и уменьшает скорость
  if (is_pressed(hpw::keycode::focus)) {
    spd = m_focus_speed;
    phys.set_force(focus_force);
  }

  return spd;
}

void Player_dark::accept_speed_boost(const real speed) {
  // во все стороны двигаться с одинаковой скоростью при зажатой фокусировке
  return_if (is_pressed(hpw::keycode::focus));

  // буст скорости при нажатии вверх
  if (pressed_up()) {
    auto vel = phys.get_vel();
    vel.y *= m_boost_up;
    phys.set_vel(vel);
    phys.set_speed( std::min(phys.get_speed(), speed * m_boost_up) );
  }

  // замедление при нажатии вниз
  if (pressed_down()) {
    auto vel = phys.get_vel();
    vel.y *= m_boost_down;
    phys.set_vel(vel);
    phys.set_speed( std::min(phys.get_speed(), speed * m_boost_down) );
  }
}

bool Player_dark::pressed_up() const {
  return is_pressed(hpw::keycode::up)
  && !is_pressed(hpw::keycode::down);
}

bool Player_dark::pressed_down() const {
  return is_pressed(hpw::keycode::down)
  && !is_pressed(hpw::keycode::up);
}

bool Player_dark::pressed_left() const {
  return is_pressed(hpw::keycode::left)
  && !is_pressed(hpw::keycode::right);
}

bool Player_dark::pressed_right() const {
  return is_pressed(hpw::keycode::right)
  && !is_pressed(hpw::keycode::left);
}

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
  for (crauto window_pos: window_pos_table) {
    // вспышка не всегда появляется
    cont_if (rndb_fast() > 16);

    // рисует звезду
    Light star;
    star.set_duration(1);
    assert(m_window_star_len > 0);
    star.radius = ratio * rndr_fast(0, m_window_star_len);
    star.flags.no_sphere = true;
    star.draw(dst, window_pos + anim_ctx.get_drawed_pos());
  }
} // draw_stars
