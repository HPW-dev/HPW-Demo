#pragma once
#include "game/entity/entity-type.hpp"
#include "util/macro.hpp"
#include "util/math/vec.hpp"

class Image;

// ресолвер коллизий (база)
class Collider {
  nocopy(Collider);

public:
  Collider() = default;
  virtual ~Collider() = default;
  virtual void operator()(CN<Entities> entities, Delta_time dt) = 0;
  inline virtual void debug_draw(Image& dst, const Vec camera_offset) {}
};
