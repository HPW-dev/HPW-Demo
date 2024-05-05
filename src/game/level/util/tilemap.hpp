#pragma once
#include "util/mem-types.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"
#include "graphic/image/color-blend.hpp"

struct Vec;
class Image;
class Yaml;

// Рисует на экране тайловый фон
class Tilemap final {
  struct Impl;
  Unique<Impl> impl {};

public:
  Tilemap();
  Tilemap(CN<Tilemap> other) = delete;
  Tilemap(Tilemap&& other);
  Tilemap* operator=(CN<Tilemap> other) = delete;
  Tilemap* operator=(Tilemap&& other) = delete;
  Tilemap(CN<Str> fname);
  ~Tilemap();
  
  void load(CN<Yaml> config);
  void load_from_archive(CN<Str> fname);
  void draw(const Vec pos, Image& dst, blend_pf bf=&blend_past, int optional=0) const;
  int get_original_w() const;
  int get_original_h() const;
  int get_tile_w() const;
  int get_tile_h() const;
}; // Tilemap
