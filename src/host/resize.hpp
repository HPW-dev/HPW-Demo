#pragma once
#include "command.hpp"
#include "game/game-window.hpp"

/// параметры ресайза окон
struct Resize_ctx {
  Resize_mode mode = Resize_mode::by_height;
  /// window size:
  int w = 0, h = 0;
  /// visible output rectangle:
  int sx=0, sy=0, ex=0, ey=0;
  bool is_bad() const;

  void resize_by_mode(int &new_w, int &new_h, int old_w, int old_h);
};
