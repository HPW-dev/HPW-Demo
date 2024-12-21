#pragma once
#include <string_view>
#include "level.hpp"
#include "util/mem-types.hpp"

// Уровень в космосе
class Level_space final: public Level {
public:
  constx std::string_view NAME {"Space"};
  
  Level_space();
  ~Level_space();
  void update(const Vec vel, Delta_time dt) override;
  void draw(Image& dst) const override;
  Str level_name() const override;

private:
  nocopy(Level_space);
  struct Impl;
  Unique<Impl> impl {};
};
