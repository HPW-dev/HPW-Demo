#pragma once
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"

struct Vec;

// камера игры
class Camera final {
  struct Impl;
  Unique<Impl> impl {};

public:
  Camera();
  ~Camera();
  Vec get_offset() const; // узнать смещение для отрисовки
  void add_shake(real intense); // добавить тряску
  void update(const Delta_time dt);
  bool shaked() const; // проверить трясётся ли камера
};

namespace graphic {
inline Shared<Camera> camera {}; // камера игрока
}
