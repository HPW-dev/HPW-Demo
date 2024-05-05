#pragma once
#include "util/mem-types.hpp"
#include "entity-loader.hpp"

class Yaml;

// Загрузчик для частиц
class Particle_loader final: public Entity_loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Particle_loader(CN<Yaml> config);
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
  ~Particle_loader();
}; // Particle_loader
