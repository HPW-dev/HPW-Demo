#pragma once
#include "util/math/num-types.hpp"

class Image;

// База для интерфейса игрока
class Hud {
public:
  Hud() = default;
  virtual ~Hud() = default;
  virtual void draw(Image& dst) const = 0;
  virtual void update(const Delta_time dt) = 0;
};
