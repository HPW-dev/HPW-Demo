#pragma once
#include "util/macro.hpp"

class Image;
struct Vec;

/// База для уровней
class Level {
  nocopy(Level);

public:
  /// if true - level complete. Use Level_mgr::finalize_level
  bool complete {false};

  Level() = default;
  virtual ~Level() = default;
  /// @param vel - смещение фона уровня (внутри dt на него не влияет)
  virtual void update(const Vec vel, double dt) = 0;
  /// отрисовка на уровне нижнего фона
  virtual void draw(Image& dst) const = 0;
  /// отрисовка на уровне выше (post draw)
  inline virtual void draw_upper_layer(Image& dst) const {}
}; // Level
