#pragma once
#include <functional>
#include "util/unicode.hpp"

using menu_item_ft = std::function<void ()>;

/// база для элементов меню
class Menu_item {
public:
  virtual ~Menu_item() = default;
  virtual void enable(); /// enable key
  virtual void plus(); /// right key
  virtual void minus(); /// left key
  virtual utf32 to_text() const;
  virtual utf32 get_description() const;
};
