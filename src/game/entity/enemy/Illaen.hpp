#pragma once
#include "game/entity/enemy/proto-enemy.hpp"
#include "util/math/timer.hpp"
#include "util/str.hpp"
#include "graphic/effect/heat-distort.hpp"

class Anim;

/// Белые космические глаза (отталкивают)
class Illaen final: public Proto_enemy {
  nocopy(Illaen);
  struct Info {
    // TODO
  } m_info {};

public:
  class Loader;
  Illaen();
  ~Illaen() = default;
  void draw(Image& dst, const Vec offset) const override;
  void update(double dt) override;
}; // Illaen

/// Загрузчик
class Illaen::Loader final: public Proto_enemy::Loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  Loader() = default;
  explicit Loader(CN<Yaml> config);
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
  ~Loader();
};
