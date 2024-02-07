#pragma once

class Image;

/// базовый класс для сцен (менюшки, катсцены, игра)
class Scene {
public:
  virtual void update(double dt);
  virtual void draw(Image& dst) const;
  Scene() = default;
  virtual ~Scene() = default;
};
