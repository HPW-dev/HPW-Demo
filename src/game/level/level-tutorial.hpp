#pragma once
#include <string_view>
#include "level.hpp"
#include "util/mem-types.hpp"

// Уровень для копипаста
class Level_tutorial final: public Level {
  nocopy(Level_tutorial);

  struct Impl;
  Unique<Impl> impl {};

public:
  constx std::string_view NAME {"Tutorial"};
  
  Level_tutorial();
  ~Level_tutorial();
  void update(const Vec vel, Delta_time dt) override;
  void draw(Image& dst) const override;
  void draw_upper_layer(Image& dst) const override;
  Str level_name() const override;
  void on_player_death(const Delta_time dt) override;
};
