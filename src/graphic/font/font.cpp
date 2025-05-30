#include <cassert>
#include <algorithm>
#include "font.hpp"

int Font_base::text_width(cr<utf32> text) const noexcept {
  int chars = 0;
  int idx = 0;

  for (auto ch: text) {
    if (ch == U'\n') {
      idx = 0;
      chars = std::max(chars, idx);
    }
    ++idx;
  }
  
  chars = std::max(chars, idx);
  return chars * (space_.x + w_);
}

int Font_base::text_height(cr<utf32> text) const noexcept {
  int strings = 1;
  // посчитать сколько переносов строки в тексте
  for (auto ch: text)
    if (ch == U'\n')
      ++strings;
  return strings * space_.y * h_;
}

Veci Font_base::text_size(cr<utf32> text) const noexcept
  { return Veci(text_width(text), text_height(text)); }

void Font_base::set_w(int w) {
  w_ = w;
  assert(w_ > 0);
}

void Font_base::set_h(int h) {
  h_ = h;
  assert(h_ > 0);
}

void Font_base::set_space(const Veci space) {
  space_ = space;
  assert(space_.x > -1);
  assert(space_.y > -1);
}
