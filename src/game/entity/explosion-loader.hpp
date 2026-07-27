#pragma once
#include "util/mem-types.hpp"
#include "entity-loader.hpp"

class Yaml;

// Загрузчик простых взрывов
class Explosion_loader final: public Entity_loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Explosion_loader(cr<Yaml> config);
  ~Explosion_loader();
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
};
