#pragma once
#include "menu.hpp"
#include "util/math/vec.hpp"

/// простое текстовое меню
class Text_menu final: public Menu {
  Vec draw_pos {};

public:
  Text_menu(CN<Menu_items> _items, const Vec _draw_pos);
  void draw(Image& dst) const override;
};
