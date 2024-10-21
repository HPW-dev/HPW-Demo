#pragma once
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"

class Image;

// рисует дебажную статистику в виде графиков
class Dbg_plots {
  struct Impl;
  Unique<Impl> impl {};

public:
  Dbg_plots();
  ~Dbg_plots();
  void draw(Image& dst) const;
  void update(Delta_time dt);
};

// отображает текущий FPS и Dt
void draw_fps_info(Image& dst);
