#pragma once
#include "util/mem-types.hpp"
#include "util/math/vec.hpp"

class Sprite;

/// хранит настройки для инициализации кадра анимации
struct Direct final {
  Weak<Sprite> sprite {}; /// кадр поворота
  Vec offset {}; /// смещение кадра

  Direct() = default;
  inline Direct(CN<Weak<Sprite>> _sprite, const Vec _offset)
    : sprite{_sprite}, offset{_offset} {}
  Direct(CN<Direct> other);
  Direct(Direct&& other);
  Direct* operator =(CN<Direct> other);
  Direct* operator =(Direct&& other);
}; // Direct
