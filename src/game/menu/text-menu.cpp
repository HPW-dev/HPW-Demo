#include "text-menu.hpp"
#include "item/item.hpp"
#include "game/core/fonts.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"

Text_menu::Text_menu(cr<Menu_items> _items, const Vec _draw_pos, cr<Text_menu_config> config)
: Menu(_items)
, _config {config}
, draw_pos(_draw_pos)
{}

inline static utf32 _process_text(cr<Text_menu> tm) {
  utf32 text;

  for (auto item: tm.get_items()) {
    text += item->to_text();
    if (item == tm.get_cur_item())
      text += U" <";
    text += U'\n';
  }

  if (text.back() == U'\n')
    text.pop_back();

  return text;
}

void Text_menu::draw(Image& dst) const {

  // нарисовать рамку позади текста
  if (_config.with_bg) {
    const Rect bg_rect = this->rect();
    draw_rect_filled(dst, bg_rect, _config.color_bg, _config.bg_bf, {});
    draw_rect(dst, bg_rect, _config.color_border, _config.border_bf, {});
  }
  
  assert(graphic::font);
  cauto text = _process_text(*this);
  graphic::font->draw(dst, draw_pos, text, _config.text_bf);
}

Recti Text_menu::rect() const {
  cauto text = _process_text(*this);
  cauto font_sz = graphic::font->text_size(text);
  const Recti ret(
    draw_pos - _config.border_offset,
    font_sz + (_config.border_offset*2));
  return ret;
}
