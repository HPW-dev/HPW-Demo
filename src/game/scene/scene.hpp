#pragma once
#include "util/math/num-types.hpp"
#include "util/str.hpp"
#include "util/macro.hpp"
#include "graphic/image/image-fwd.hpp"

// базовый класс для сцен (менюшки, катсцены, игра)
class Scene {
public:
  constx Str NAME = "base scene";

  Scene() = default;
  virtual ~Scene() = default;
  virtual void update(const Delta_time dt);
  virtual void draw(Image& dst) const;
  virtual Str name() const = 0;
};
