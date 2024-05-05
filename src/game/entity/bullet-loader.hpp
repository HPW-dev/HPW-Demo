#pragma once
#include "util/mem-types.hpp"
#include "entity-loader.hpp"

class Yaml;

// Загрузчик пуль
class Bullet_loader final: public Entity_loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Bullet_loader(CN<Yaml> config);
  ~Bullet_loader();
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
};
