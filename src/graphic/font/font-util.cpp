#include <cassert>
#include "font-util.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"

void draw_center(cp<Font_base> font, Image& dst,
cr<utf32> msg, blend_pf bf) {
  auto w = font->text_width(msg);
  auto h = font->text_height(msg);
  Vec pos{
    dst.X / 2 - w / 2,
    dst.Y / 2 - h / 2,
  };
  font->draw(dst, pos, msg, bf);
}

void text_bordered(Image& dst, cr<utf32> txt, cp<Font_base> font,
const Rect border, const Vec offset, blend_pf bf, int opt) {
  assert(dst);
  assert(font);
  assert(bf);
  return_if(txt.empty());
  return_if(border.size.is_zero());
  assert(border.size.x >= 0);
  assert(border.size.y >= 0);

  cauto separated = split(txt, U' ');
  const int needed_str_width = border.size.x - std::abs(offset.x);
  const int needed_str_height = border.size.y - std::abs(offset.y);
  return_if(needed_str_width <= 0);
  return_if(needed_str_height <= 0);
  utf32 formated_txt;
  utf32 str;
  bool stop {}; // true - текст по высоте вышел за пределы

  // узнавать размер каждого слова и ставить EOL где нужно
  for (auto word: separated) {
    cauto str_width = font->text_width(str + word);
    if (str_width >= needed_str_width) {
      formated_txt += str + U'\n';
      str.clear();
    }
    if (font->text_height(formated_txt) >= needed_str_height) {
      stop = true;
      break;
    }
    str += word + U' ';
  }
  if (stop) {
    // удалить '\n'
    if (!formated_txt.empty())
      formated_txt.pop_back();
    formated_txt += U"...";
  } else {
    formated_txt += str;
  }
  
  const Vec pos = border.pos + offset;
  font->draw(dst, pos, formated_txt, bf, opt);
}
