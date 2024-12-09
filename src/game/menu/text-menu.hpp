#pragma once
#include "menu.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/color-blend.hpp"

struct Text_menu_config {
  bool with_bg {}; // включает прямоуголник позади текста
  Pal8 color_bg {Pal8::black};
  Pal8 color_border {Pal8::white};
  blend_pf bg_bf {&blend_158};
  blend_pf border_bf {&blend_diff};
  blend_pf text_bf {&blend_max};
  int border_offset {9}; // отступ от текста и рамки
};

// простое текстовое меню
class Text_menu final: public Menu {
  Vec draw_pos {};
  
public:
  Text_menu_config _config {};

  Text_menu(cr<Menu_items> _items, const Vec _draw_pos={}, cr<Text_menu_config> config={});
  void draw(Image& dst) const override;
};
