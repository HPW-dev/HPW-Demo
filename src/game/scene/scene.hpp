#pragma once
#include "util/math/num-types.hpp"

class Image;

// базовый класс для сцен (менюшки, катсцены, игра)
class Scene {
public:
  virtual void update(const Delta_time dt);
  virtual void draw(Image& dst) const;
  Scene() = default;
  virtual ~Scene() = default;
};
