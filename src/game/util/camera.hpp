#pragma once
#include "util/mem-types.hpp"

struct Vec;

/// камера игры
class Camera final {
  struct Impl;
  Unique<Impl> impl {};

public:
  Camera();
  ~Camera();
  Vec get_offset() const; /// узнать смещение для отрисовки
  void add_shake(double intense); /// добавить тряску
  void update(double dt);
};

namespace graphic {
inline Shared<Camera> camera {}; /// камера игрока
}
