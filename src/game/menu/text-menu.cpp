#include "text-menu.hpp"
#include "item/item.hpp"
#include "game/core/fonts.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"

Text_menu::Text_menu(cr<Menu_items> _items, const Vec _draw_pos, cr<Text_menu_config> config)
: Menu(_items)
, draw_pos(_draw_pos)
, _config {config}
{}

void Text_menu::draw(Image& dst) const {
  utf32 text;
  for (auto item: Menu::get_items()) {
    text += item->to_text();
    if (item == Menu::get_cur_item())
      text += U" <";
    text += U'\n';
  }
  if (text.back() == U'\n')
    text.pop_back();

  // нарисовать рамку позади текста
  if (_config.with_bg) {
    cauto font_sz = graphic::font->text_size(text);
    const Rect bg_rect(draw_pos - _config.border_offset, font_sz + (_config.border_offset*2));
    draw_rect_filled(dst, bg_rect, _config.color_bg, _config.bg_bf, {});
    // рамка
    draw_rect(dst, bg_rect, _config.color_border, _config.border_bf, {});
  }
  
  assert(graphic::font);
  graphic::font->draw(dst, draw_pos, text, _config.text_bf);
}
