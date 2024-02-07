#pragma once
#include "game/entity/enemy/proto-enemy.hpp"

/// Стреляет лазером и отскакивает. Убивать, когда он открыт
class Cosmic_beamer final: public Proto_enemy {
  nocopy(Cosmic_beamer);
  
public:
  Cosmic_beamer();
  ~Cosmic_beamer() = default;

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

}; // Cosmic_beamer
