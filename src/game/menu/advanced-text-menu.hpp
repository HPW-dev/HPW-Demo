
#pragma once
#include "menu.hpp"
#include "util/math/rect.hpp"
#include "util/unicode.hpp"

// Более красивое текстовое меню с описанием
class Advanced_text_menu: public Menu {
  struct Impl;
  Unique<Impl> impl {};

public:
  ~Advanced_text_menu();
  explicit Advanced_text_menu(cr<utf32> title, cr<Menu_items> items, const Rect rect, bool without_desc=false);
  void draw(Image& dst) const override;
  void update(const Delta_time dt) override;
};
