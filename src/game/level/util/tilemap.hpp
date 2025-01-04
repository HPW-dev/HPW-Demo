#pragma once
#include "util/mem-types.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/color-blend.hpp"

class Image;
class Yaml;

// Рисует на экране тайловый фон
class Tilemap final {
  struct Impl;
  Unique<Impl> impl {};

public:
  Tilemap();
  Tilemap(cr<Tilemap> other) = delete;
  Tilemap(Tilemap&& other);
  Tilemap* operator=(cr<Tilemap> other) = delete;
  Tilemap* operator=(Tilemap&& other) = delete;
  Tilemap(cr<Str> fname, const bool is_archive = true);
  ~Tilemap();
  
  void draw(const Vec pos, Image& dst, blend_pf bf=&blend_past, int optional=0) const;
  int get_original_w() const;
  int get_original_h() const;
  int get_tile_w() const;
  int get_tile_h() const;
}; // Tilemap
