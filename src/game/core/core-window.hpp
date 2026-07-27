#pragma once

// режимы растягивания вывода
enum class Resize_mode {
  one_to_one = 0, // масштаб 1:1
  by_width,       // Y aspected, X resizeble
  by_height,      // Y resizeble, X aspected
  full,           // пролное растягивание по X и Y

  max_num
};

namespace graphic {
inline Resize_mode resize_mode = Resize_mode::one_to_one;
inline Resize_mode default_resize_mode = Resize_mode::one_to_one;
}
