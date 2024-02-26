#pragma once
#include "game/entity/enemy/proto-enemy.hpp"
#include "util/math/timer.hpp"
#include "util/str.hpp"
#include "graphic/effect/heat-distort.hpp"

class Anim;

/// Космические глаза
class Cosmic final: public Proto_enemy {
  nocopy(Cosmic);

  struct Info {
    Anim* state_1 {}; /// анимация на стадии появления
    Anim* state_2 {}; /// основная анимация с хитбоксом
    Anim* contour {};
    Timer fade_in_timer {}; /// время на появление из темноты
    Timer eyes_open_timeout {}; /// время на анимацию открытия глаз
    Timer shoot_timer {}; /// таймер выстрела
    real magnet_range {};
    real magnet_power {};
    real bullet_spawn_range {};
    real bullet_speed {};
    real bullet_accel {};
    real bullet_predict_speed {}; /// начальная скорость пули для предикта
    Str bullet {};
    Heat_distort heat_distort {};
    int bullet_count {}; /// сколько пустить пуль за выстрел
  } m_info {};

  bool m_fade_in_complete {false};
  bool m_eyes_open_complete {false};

public:
  class Loader;

  Cosmic();
  ~Cosmic() = default;

  void draw(Image& dst, const Vec offset) const override;
  void update(double dt) override;
}; // Cosmic

/// Загрузчик
class Cosmic::Loader final: public Proto_enemy::Loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  Loader() = default;
  explicit Loader(CN<Yaml> config);
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
  ~Loader();
};
