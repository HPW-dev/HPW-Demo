#pragma once
#include "game/entity/collidable.hpp"
#include "game/entity/entity-loader.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image-fwd.hpp"

class Yaml;

// общая база для изичного построения врагов
class Proto_enemy: public Collidable {
  nocopy(Proto_enemy);
public:
  Proto_enemy();
  explicit Proto_enemy(Entity_type new_type);
  ~Proto_enemy() = default;

  // Загрузчик
  class Loader: public Entity_loader {
    struct Impl;
    Unique<Impl> impl {};

  public:
    Loader() = default;
    explicit Loader(cr<Yaml> config);
    Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
    ~Loader();
  };

}; // Proto_enemy