#pragma once
#include <thirdparty/include/nlohmann/json_fwd.hpp>
#include "util/mem-types.hpp"
#include "entity-loader.hpp"

// Загрузчик для частиц
class Particle_loader final: public Entity_loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Particle_loader(cr<nlohmann::json> config);
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
  ~Particle_loader();
};
