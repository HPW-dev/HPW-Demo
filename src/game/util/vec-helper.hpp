#pragma once
#include "util/math/num-types.hpp"

struct Rect;
struct Vec;

[[nodiscard]] Vec get_screen_center();
[[nodiscard]] Vec get_rand_pos_safe(const real sx, const real sy, const real ex, const real ey);
[[nodiscard]] Vec get_rand_pos_graphic(const real sx, const real sy, const real ex, const real ey);
[[nodiscard]] Vec rnd_screen_pos_safe(); // случайная позиция на экране
[[nodiscard]] Vec rnd_screen_pos_fast(); // случайная позиция на экране (для графики)
[[nodiscard]] Rect get_screen_rect();
