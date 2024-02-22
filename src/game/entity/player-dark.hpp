#pragma once
#include "player.hpp"
#include "entity-loader.hpp"
#include "util/mem-types.hpp"
#include "util/math/timer.hpp"

class Yaml;
class Anim;

/// Стартовый самолёт игрока
class Player_dark final: public Player {
  nocopy(Player_dark);
  void check_input(double dt);
  void shoot(double dt);
  void sub_en(hp_t val);
  void energy_regen();
  void blink_contour() const; /// мерцание контура
  void draw_stars(Image& dst) const; /// звёздочки на окошках игрока

public:
  /// Загрузчик для игрока
  class Loader;
  
  Timer m_shoot_timer {}; /// задержка на обычный выстрел
  real m_max_speed {}; /// макс скорость обычного полёта
  real m_focus_speed {}; /// скорость при фокусировке
  hp_t m_shoot_price {}; /// стоимсть за выстрел (энергия)
  hp_t m_energy_regen {}; /// скорость восстановления энергии
  hp_t m_power_shoot_price {}; /// цена мощного выстрела
  hp_t m_energy_for_power_shoot {}; /// сколько должно быть энергии для мощного выстрела

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
