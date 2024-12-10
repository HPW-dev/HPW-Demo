
#pragma once
#include "menu.hpp"
#include "util/math/rect.hpp"
#include "util/unicode.hpp"
#include "graphic/image/color.hpp"
#include "graphic/image/color-blend.hpp"

struct Advanced_text_menu_config {
  bool without_desc {};
  Pal8 color_border {Pal8::white};
  Pal8 color_bg {Pal8::black};
  blend_pf bf_border {&blend_past};
  blend_pf bf_bg {&blend_past};
};

// Более красивое текстовое меню с описанием
class Advanced_text_menu: public Menu {
  struct Impl;
  Unique<Impl> impl {};

public:
  ~Advanced_text_menu();
  explicit Advanced_text_menu(cr<utf32> title, cr<Menu_items> items, const Rect rect,
    cr<Advanced_text_menu_config> config = {});
  void draw(Image& dst) const override;
  void update(const Delta_time dt) override;
};
