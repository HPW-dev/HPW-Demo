#pragma once
#include <thirdparty/include/nlohmann/json_fwd.hpp>
#include "util/mem-types.hpp"
#include "entity-loader.hpp"

// Загрузчик бонусов
class Bonus_loader final: public Entity_loader {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Bonus_loader(cr<nlohmann::json> config);
  ~Bonus_loader();
  Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) override;
};
