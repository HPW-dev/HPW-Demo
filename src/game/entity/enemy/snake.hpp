#pragma once
#include "game/entity/enemy/proto-enemy.hpp"
#include "util/math/timer.hpp"

class Anim;

// голова змеи
class Enemy_snake_head final: public Proto_enemy {
  nocopy(Enemy_snake_head);
  struct Info {
    Str tail_name {}; // имя объектов из которых состоит хвост змеи
    uint tail_count {};
    real speed {};
    real kill_delay {}; // через какое время уничтожить хвосты после смерти головы
    bool enable_scatter_if_head_death {}; // if true - разлетаться по сторонам после смерти головы
  } m_info {};
public:
  class Loader;
  Enemy_snake_head();
  ~Enemy_snake_head() = default;
  void update(const Delta_time dt) override;
};

// хвост змеи
class Enemy_snake_tail final: public Proto_enemy {
  nocopy(Enemy_snake_tail);
  struct Info {
    real start_motion_radius {}; // пока хвост в пределах этого радиуса, хвост не двигается
  } m_info {};
  
  Vec m_target {}; // сюда следует хвост

public:
  class Loader;
  Enemy_snake_tail();
  ~Enemy_snake_tail() = default;
  void update(const Delta_time dt) override;
};

// Загрузчик головы
class Enemy_snake_head::Loader final: public Proto_enemy::Loader {
  struct Impl;
  Unique<Impl> impl {};
public:
  Loader() = default;
  explicit Loader(CN<Yaml> config);
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
  ~Loader();
};

// Загрузчик хвоста
class Enemy_snake_tail::Loader final: public Proto_enemy::Loader {
  struct Impl;
  Unique<Impl> impl {};
public:
  Loader() = default;
  explicit Loader(CN<Yaml> config);
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
  ~Loader();
};
