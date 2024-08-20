#pragma once
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"
#include "util/math/vec.hpp"

class Player;
class Image;

// от этого надо наследоваться, чтобы получить способности
class Ability_entry {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Ability_entry(Player& player);
  ~Ability_entry();
  void update(Delta_time dt);
  void clear(); // убрать все способности
  void draw_bg(Image& dst, const Vec offset) const;
  void draw_fg(Image& dst, const Vec offset) const;
};
