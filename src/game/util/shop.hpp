#pragma once
#include "game/util/task.hpp"

// интерактивное меню покупки способностей
class Shop_task final: public Task {
  struct Impl;
  Unique<Impl> impl {};

public:
  Shop_task();
  ~Shop_task();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
