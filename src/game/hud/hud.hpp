#pragma once

class Image;

/// База для интерфейса игрока
class Hud {
public:
  Hud() = default;
  virtual ~Hud() = default;
  virtual void draw(Image& dst) const = 0;
  virtual void update(double dt) const = 0;
};
