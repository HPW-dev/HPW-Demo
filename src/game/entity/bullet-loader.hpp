#pragma once
#include <thirdparty/include/nlohmann/json_fwd.hpp>
#include "util/mem-types.hpp"
#include "entity-loader.hpp"

// Загрузчик пуль
class Bullet_loader final: public Entity_loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Bullet_loader(cr<nlohmann::json> config);
  ~Bullet_loader();
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
};
