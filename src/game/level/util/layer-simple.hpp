#pragma once
#include "tilemap.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/color-blend.hpp"
#include "util/math/num-types.hpp"
#include "util/str.hpp"

struct Layer_simple {
  Tilemap tilemap {};
  Vec pos {};
  real motion_ratio {1.0}; // влияет на скорость движения фона
  blend_pf bf {&blend_past};

  Layer_simple() = default;
  explicit Layer_simple(cr<Str> tilemap_archive, const Vec _pos,
    real _motion_ratio, blend_pf _bf=&blend_past);
  Layer_simple(Layer_simple&& other);
  void update(const Vec vel, Delta_time dt);
  void draw(Image& dst, int optional=0) const;
};
