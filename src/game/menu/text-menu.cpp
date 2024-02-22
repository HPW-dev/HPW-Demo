#include "text-menu.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "item/item.hpp"
#include "game/core/fonts.hpp"

Text_menu::Text_menu(CN<Menu_items> _items, const Vec _draw_pos)
: Menu(_items), draw_pos(_draw_pos) {}

void Text_menu::draw(Image& dst) const {
  utf32 text;
  for (auto item: m_items) {
    text += item->to_text();
    if (item == m_items[m_cur_item])
      text += U" <";
    text += U"\n";
  }
  
  //dst.fill(Pal8::black);
  graphic::font->draw(dst, draw_pos, text, &blend_max);
}
