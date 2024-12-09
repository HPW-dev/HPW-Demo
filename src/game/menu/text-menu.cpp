#include "text-menu.hpp"
#include "graphic/image/image.hpp"
#include "item/item.hpp"
#include "game/core/fonts.hpp"

Text_menu::Text_menu(cr<Menu_items> _items, const Vec _draw_pos, blend_pf bf)
: Menu(_items)
, draw_pos(_draw_pos)
, _bf {bf}
{}

void Text_menu::draw(Image& dst) const {
  utf32 text;
  for (auto item: Menu::get_items()) {
    text += item->to_text();
    if (item == Menu::get_cur_item())
      text += U" <";
    text += U"\n";
  }
  
  graphic::font->draw(dst, draw_pos, text, _bf);
}
