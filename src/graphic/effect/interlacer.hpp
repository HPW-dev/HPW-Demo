#pragma once
#include "util/math/num-types.hpp"
#include "util/mem-types.hpp"

class Image;

/// эффект поочерёдного отображения пикселей (2D интерлейсинг)
class Interlacer final {
  struct Impl;
  Shared<Impl> impl {};

public:
  /** @param mx отступ вставки по ширине
  *   @param my отступ вставки по высоте
  *   @param screen_w ширина целевого кадра
  *   @param screen_h высота целевого кадра */
  Interlacer(uint mx, uint my, uint screen_w, uint screen_h);
  Interlacer(const Interlacer& other);
  ~Interlacer() = default;
  void operator()(Image& dst) const;
};
