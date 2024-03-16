#pragma once
#include "util/macro.hpp"
#include "util/mem-types.hpp"

class Image;
struct Vec;

/// База для уровней
class Level {
  nocopy(Level);
  struct Impl;
  Unique<Impl> impl {};

public:
  /// if true - level complete. Use Level_mgr::finalize_level
  bool m_complete {false};

  Level();
  virtual ~Level();
  /// @param vel - смещение фона уровня (внутри dt на него не влияет)
  virtual void update(const Vec vel, double dt);
  /// отрисовка на уровне нижнего фона
  virtual void draw(Image& dst) const = 0;
  /// отрисовка на уровне выше (post draw)
  inline virtual void draw_upper_layer(Image& dst) const {}
}; // Level
