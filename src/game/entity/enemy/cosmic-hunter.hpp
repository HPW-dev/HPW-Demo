#pragma once
#include "game/entity/enemy/proto-enemy.hpp"
#include "util/math/timer.hpp"

/// Простой противник стреляющий на упреждение в игрока
class Cosmic_hunter final: public Proto_enemy {
  nocopy(Cosmic_hunter);
  Timer shoot_timer {};
  real m_shoot_timer_max {};
  real m_rotate_speed {};
  real m_bullet_speed {};
  real m_shoot_deg {}; /// разброс пуль
  
public:
  Cosmic_hunter();
  ~Cosmic_hunter() = default;

  void draw(Image& dst, const Vec offset) const override;
  void update(double dt) override;

  /// Загрузчик
  class Loader final: public Proto_enemy::Loader {
    struct Impl;
    Unique<Impl> impl {};

  public:
    Loader() = default;
    explicit Loader(CN<Yaml> config);
    Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
    ~Loader();
  };

}; // Cosmic_hunter