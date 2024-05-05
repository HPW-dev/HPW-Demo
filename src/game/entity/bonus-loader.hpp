#pragma once
#include "util/mem-types.hpp"
#include "entity-loader.hpp"

class Yaml;

// Загрузчик бонусов
class Bonus_loader final: public Entity_loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Bonus_loader(CN<Yaml> config);
  ~Bonus_loader();
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
};
