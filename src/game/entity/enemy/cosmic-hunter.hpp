#pragma once
#include "game/entity/enemy/proto-enemy.hpp"
#include "util/math/timer.hpp"
#include "util/str.hpp"
#include "game/entity/util/hitbox.hpp"

// Простой противник стреляющий на упреждение в игрока
class Cosmic_hunter final: public Proto_enemy {
  nocopy(Cosmic_hunter);
  struct Info {
    Anim_ctx external_part {}; // анимация внешней части
    Str bullet_name {};
    Timer shoot_timer {};
    bool external_rot_dir {}; // true - крутиться вправо
    real external_deg {};
    real shoot_timer_max {};
    real rotate_speed {};
    real bullet_speed {};
    real shoot_deg {}; // разброс пуль
    real speed {};
    real initial_rot_spd {}; // с какой скоростью крутится внешняя часть
  };
  Info _info {};
  Hitbox _hitbox {}; // хитбокс из двух частей
  
public:
  class Loader;
  Cosmic_hunter();
  ~Cosmic_hunter() = default;

  void draw(Image& dst, const Vec offset) const override;
  void update(const Delta_time dt) override;
  cp<Hitbox> get_hitbox() const override;
};

// Загрузчик
class Cosmic_hunter::Loader final: public Proto_enemy::Loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  Loader() = default;
  explicit Loader(cr<Yaml> config);
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
  ~Loader();
};
