#include <cassert>
#include "unifont-mono.hpp"
#include "graphic/util/util-templ.hpp"

Shared<Unifont::Glyph> Unifont_mono::_load_glyph(char32_t ch) const {
  auto glyph = Unifont::_load_glyph(ch);
  return_if(!glyph, {});
  // сделать ширину глифа такой же, какая и высота глифа
  assert(glyph->image);
  Sprite extended_glyph(glyph->image.Y(), glyph->image.Y());
  const Vec past_pos((extended_glyph.X() - glyph->image.X()) / 2, 0);
  insert(extended_glyph.image(), glyph->image.image(), past_pos);
  glyph->image = extended_glyph;
  glyph->xoff += past_pos.x;
  return glyph;
}
