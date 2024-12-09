#pragma once
#include "menu.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/color-blend.hpp"

// простое текстовое меню
class Text_menu final: public Menu {
  Vec draw_pos {};
  blend_pf _bf {&blend_max};

public:
  Text_menu(cr<Menu_items> _items, const Vec _draw_pos={}, blend_pf bf = &blend_max);
  void draw(Image& dst) const override;
};
