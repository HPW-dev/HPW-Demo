#pragma once

#include <deque>
#include <functional>

#include "engine/graphic/image/image-fwd.hpp"
#include "util/num-types.hpp"
#include "util/unicode.hpp"
#include "util/vec.hpp"


// рисует графики
class Plotter {
public:
  using Value = real;
  using Getter = std::function<Value ()>;

  /** окошко с графиком
  @param title название для графика
  @param getter получает значения для графика при вызове update
  @param w ширина окна с графиком
  @param h высота окна с графиком
  @param len разрешение графика. Если = 0, разрешение станет = w
  @param offset с каким смещением будет отрисовано окно графика */
  explicit Plotter(cr<utf32> title, Getter&& getter,
    uint w, uint h, uint len, const Vec offset);

  void update();
  void draw(Image& dst) const;

private:
  utf32 m_title {};
  Getter m_getter {};
  uint m_w {};
  uint m_h {};
  Vec m_offset {};
  std::deque<Value> m_values {};
  real m_maximum {};

  void update_maximum();
};
