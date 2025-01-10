#pragma once
#include <functional>
#include "util/unicode.hpp"
#include "util/math/num-types.hpp"

// база для элементов меню
class Menu_item {
public:
  using Action = std::function<void ()>;
  
  virtual ~Menu_item() = default;
  virtual inline void update(const Delta_time dt) {}
  virtual void enable(); // enable key
  virtual void plus(); // right key
  virtual void plus_fast(); // holded right key
  virtual void minus(); // left key
  virtual void minus_fast(); // holded left key
  virtual utf32 to_text() const;
  virtual utf32 get_description() const;
};
