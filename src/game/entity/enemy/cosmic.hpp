#pragma once
#include "game/entity/enemy/proto-enemy.hpp"
#include "util/math/timer.hpp"
#include "util/str.hpp"

class Anim;

/// Космические глаза
class Cosmic final: public Proto_enemy {
  nocopy(Cosmic);
  Anim* m_state_1 {}; /// анимация на стадии появления
  Anim* m_state_2 {}; /// основная анимация с хитбоксом
  Anim* m_contour {};
  Timer m_fade_in_timer {}; /// время на появление из темноты
  Timer m_eyes_open_timeout {}; /// время на анимацию открытия глаз
  Timer m_shoot_timer {}; /// таймер выстрела
  real m_magnet_range {};
  real m_magnet_power {};
  real m_bullet_spawn_range {};
  real m_bullet_speed {};
  real m_bullet_accel {};
  Str m_bullet {};

  bool m_fade_in_complete {false};
  bool m_eyes_open_complete {false};

public:
  Cosmic();
  ~Cosmic() = default;

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

}; // Cosmic