#pragma once
#include "game/entity/enemy/proto-enemy.hpp"

class Anim;

// Противник для туториала
class Enemy_tutorial final: public Proto_enemy {
  nocopy(Enemy_tutorial);

  struct Info {
    
  } m_info {};

public:
  class Loader;
  Enemy_tutorial();
  ~Enemy_tutorial() = default;
  void update(const Delta_time dt) override;
}; // Enemy_tutorial

// Загрузчик
class Enemy_tutorial::Loader final: public Proto_enemy::Loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  Loader() = default;
  explicit Loader(CN<Yaml> config);
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
  ~Loader();
};
