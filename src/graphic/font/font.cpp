#include <algorithm>
#include "font.hpp"

int hpw::Font::text_width(cr<utf32> text) const {
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
  return chars * space_.x * w_;
}

int hpw::Font::text_height(cr<utf32> text) const {
  int strings = 1;
  // посчитать сколько переносов строки в тексте
  for (auto ch: text)
    if (ch == U'\n')
      ++strings;
  return strings * space_.y * h_;
}

Vec hpw::Font::text_size(cr<utf32> text) const
  { return Vec(text_width(text), text_height(text)); }
