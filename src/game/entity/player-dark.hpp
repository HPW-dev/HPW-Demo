#pragma once
#include "player.hpp"
#include "entity-loader.hpp"
#include "util/mem-types.hpp"
#include "util/math/timer.hpp"

class Yaml;
class Anim;

// Стартовый самолёт игрока
class Player_dark final: public Player {
  nocopy(Player_dark);
  void check_input(double dt);
  void shoot(double dt);
  void sub_en(hp_t val);
  void energy_regen();
  void blink_contour() const; // мерцание контура
  void draw_stars(Image& dst) const; // звёздочки на окошках игрока
  void power_shoot(double dt);
  void default_shoot(double dt);
  void move(double dt);

public:
  // Загрузчик для игрока
  class Loader;
  
  Timer m_shoot_timer {}; // задержка на обычный выстрел
  real m_max_speed {}; // макс скорость обычного полёта
  real m_focus_speed {}; // скорость при фокусировке
  hp_t m_shoot_price {}; // стоимсть за выстрел (энергия)
  hp_t m_energy_regen {}; // скорость восстановления энергии
  hp_t m_power_shoot_price {}; // цена мощного выстрела
  hp_t m_energy_for_power_shoot {}; // сколько должно быть энергии для мощного выстрела
  hp_t m_energy_level_for_decrease_shoot_speed {}; // если энергия ниже этого урвоня, то замедлить стрельбу
  real m_decrease_shoot_speed_ratio {}; // на сколько замедлить скорость стрельбы при нехватке энергии
  uint m_default_shoot_count {}; // сколько за раз выпустить пуль при обычной атаке
  real m_deg_spread_shoot {}; // угол разброса при обычном выстреле
  real m_deg_focused_shoot {}; // угол разброса при фокусировке
  real m_shoot_speed {}; // скорость обычных выстрелов
  real m_boost_up {}; // ускорение вперёд
  real m_boost_down {}; // ускорение назад
  real m_level_for_blink {}; // уровень энергии, при котором игрок мигает ярче
  real m_window_star_len {}; // размер звёздочек на окошках бумера

  Player_dark();
  ~Player_dark() = default;
  void draw(Image& dst, const Vec offset) const override;
  void update(double dt) override;
}; // Player_dark

class Player_dark::Loader final: public Entity_loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Loader(CN<Yaml> config);
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
  ~Loader();
};
