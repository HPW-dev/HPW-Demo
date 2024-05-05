#pragma once
#include "game/entity/enemy/proto-enemy.hpp"
#include "util/math/timer.hpp"
#include "util/str.hpp"

// Простой противник стреляющий на упреждение в игрока
class Cosmic_hunter final: public Proto_enemy {
  nocopy(Cosmic_hunter);
  struct Info {
    Timer shoot_timer {};
    real shoot_timer_max {};
    real rotate_speed {};
    real bullet_speed {};
    real shoot_deg {}; // разброс пуль
    real speed {};
    Str bullet_name {};
  } m_info {};
  
public:
  class Loader;
  Cosmic_hunter();
  ~Cosmic_hunter() = default;

  void draw(Image& dst, const Vec offset) const override;
  void update(double dt) override;
}; // Cosmic_hunter

// Загрузчик
class Cosmic_hunter::Loader final: public Proto_enemy::Loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  Loader() = default;
  explicit Loader(CN<Yaml> config);
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
  ~Loader();
};
