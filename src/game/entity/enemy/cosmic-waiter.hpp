#pragma once
#include "game/entity/enemy/proto-enemy.hpp"

// Простой противник стреляющий на упреждение в игрока
class Cosmic_waiter final: public Proto_enemy {
  nocopy(Cosmic_waiter);
  
public:
  Cosmic_waiter();
  ~Cosmic_waiter() = default;

  void draw(Image& dst, const Vec offset) const override;
  void update(const Delta_time dt) override;

  // Загрузчик
  class Loader final: public Proto_enemy::Loader {
    struct Impl;
    Unique<Impl> impl {};

  public:
    Loader() = default;
    explicit Loader(cr<Yaml> config);
    Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
    ~Loader();
  };

}; // Cosmic_waiter