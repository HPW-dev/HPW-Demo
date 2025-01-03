#pragma once
#include <string_view>
#include "level.hpp"
#include "util/mem-types.hpp"

// Пустой уровень для тестов
class Level_empty final: public Level {
public:
  constx std::string_view NAME {"Empty"};

  Level_empty();
  ~Level_empty();
  void update(const Vec vel, Delta_time dt) override;
  void draw(Image& dst) const override;
  Str level_name() const override;

private:
  nocopy(Level_empty);
  struct Impl;
  Unique<Impl> impl {};
};
