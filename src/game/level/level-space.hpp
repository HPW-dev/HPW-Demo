#pragma once
#include <string_view>
#include "level.hpp"
#include "util/mem-types.hpp"

/// Уровень в космосе
class Level_space final: public Level {
  nocopy(Level_space);
  constx std::string_view NAME {"Space"};

  struct Impl;
  Unique<Impl> impl {};

public:
  Level_space();
  ~Level_space();
  void update(const Vec vel, double dt) override;
  void draw(Image& dst) const override;
  Str level_name() const override;
};
