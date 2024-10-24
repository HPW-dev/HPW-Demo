#include <cassert>
#include "unifont-mono.hpp"
#include "graphic/util/util-templ.hpp"

Unifont_mono::Unifont_mono(cr<Str> fname, int width, int height, bool mono)
: Unifont_mono( File{mem_from_file(fname), fname}, height, mono, width) {}

Unifont_mono::Unifont_mono(cr<File> file, int width, int height, bool mono) {
  assert(width >= 0);
  Font::w_ = width == 0 ? height : width;
  init(file, height, mono);
}

Shared<Unifont::Glyph> Unifont_mono::_load_glyph(char32_t ch) const {
  auto glyph = Unifont::_load_glyph(ch);
  return_if(!glyph, {});
  
  // сделать ширину глифа такой же как и высота шрифта
  assert(glyph->image);
  Sprite extended_glyph(Font::w_, Font::h_);
  const Vec past_pos ((extended_glyph.X() - glyph->image.X()) / 2, 0);
  insert(extended_glyph.image(), glyph->image.image(), past_pos);
  insert(extended_glyph.mask(), glyph->image.mask(), past_pos);
  glyph->image = extended_glyph;
  //glyph->xoff -= past_pos.x;
  return glyph;
}
