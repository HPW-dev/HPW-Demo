extern "C" {
#define STB_TRUETYPE_IMPLEMENTATION 
#include <stb/stb_truetype.h>
}
#include <algorithm>
#include <memory>
#include "unifont.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/str-util.hpp"
#include "util/file/file.hpp"

struct Glyph {
  Sprite image {};
  int xoff {}, yoff {};
};

Unifont::Unifont(cr<Str> fname, int height, bool mono)
: Unifont( File{mem_from_file(fname), fname}, height, mono) {
  detailed_log("Unifont: loading (file):\""<< fname <<"\"\n");
}

Unifont::Unifont(cr<File> file, int height, bool mono)
: mono_(mono), font_file_mem_(file.data) {
  detailed_log("Unifont.c-tor: loading font\n");
  w_ = 0; // auto
  h_ = height;
  space_ = {1, 1};
  init_shared(info_);
  iferror( !stbtt_InitFont(info_.get(), font_file_mem_.data(), 0),
    "Unifont: stbtt_InitFont error");
  scale_ = stbtt_ScaleForPixelHeight(info_.get(), height);
  // предзагрузка ASCI:
  for (int i = 21; i < 128; ++i)
    _load_glyph(i);
} // Unifontmem  c-tor

int Unifont::text_width(cr<utf32> text) const {
  int size = 0;
  int max_size = 0;
  for (auto ch: text) {
    if (ch == U'\n') {
      max_size = std::max(size, max_size);
      size = 0;
    }
    size += _get_glyph(ch)->image.X() + space_.x;
  }
  max_size = std::max(size, max_size);
  return max_size + 1;
  
} // text_width


void Unifont::draw(Image& dst, const Vec pos, cr<utf32> text,
blend_pf bf, const int optional) const {
  int posx = pos.x;
  int posy = pos.y;
  posy += h_ / 2 + 2; // компенсация оффсета
  for (uint limit = 0; auto ch: text) {
    ++limit;
    // столько текста на экран не влезет
    break_if(limit >= 5'000);
    // пропуск строки
    if (ch == U'\n') {
      posy += h_ + space().y;
      posx = pos.x;
      continue;
    }
    // \r игнорить
    cont_if (ch == '\r');

    auto glyph = _get_glyph(ch);
    if ( !glyph)
      continue;
    insert(dst, glyph->image, {posx + glyph->xoff, posy + glyph->yoff}, bf, optional);
    posx += glyph->image.X() + space().x;
  } // for text size
} // draw

cp<Glyph> Unifont::_get_glyph(char32_t ch) const {
  // найти символ в кэше
  try {
    return glyph_table.at(ch).get();
  }
  // если нет символа, загрузить или вернуть null
  catch (...) {
    if ( !_load_glyph(ch))
      return {};
    return glyph_table.at(ch).get();
  }
  return {};
} // _get_glyph

bool Unifont::_load_glyph(char32_t ch) const {
  // пробел - просто путая картинка
  if (ch == U' ') {
    int ax, lsb;
    stbtt_GetCodepointHMetrics(info_.get(), ' ', &ax, &lsb);
    init_shared(glyph_table[ch]);
    auto& glyph = glyph_table.at(ch);
    glyph->image.init(ax * scale_, 1);
    glyph->image.image().fill(Pal8::black);
    glyph->yoff = -1;
    return true;
  }
  int bitmap_w, bitmap_h, bitmap_xoff, bitmap_yoff;
  auto bitmap = stbtt_GetCodepointBitmap(info_.get(), scale_, scale_, ch,
    &bitmap_w, &bitmap_h, &bitmap_xoff, &bitmap_yoff);
  if ( !bitmap) {
    detailed_log("stbtt_GetCodepointBitmap error ("<< std::hex << int(ch) << ")\n");
    return false;
  }
  init_shared(glyph_table[ch]);
  auto& glyph = glyph_table.at(ch);
  glyph->image.init(bitmap_w, bitmap_h);
  glyph->image.image().fill(Pal8::black);
  glyph->xoff = 0; // мне не нравится с bitmap_xoff
  glyph->yoff = bitmap_yoff;
  if (mono_) {
    cfor (y, bitmap_h)
    cfor (x, bitmap_w) {
      crauto pix = bitmap[y * bitmap_w + x];
      glyph->image.image().fast_set(x, y, pix > 127 ? Pal8::white : Pal8::black, {});
      glyph->image.mask().fast_set(x, y, pix > 127 ? Pal8::mask_visible : Pal8::mask_invisible, {});
    }
  } else {
    cfor (y, bitmap_h)
    cfor (x, bitmap_w) {
      crauto pix = bitmap[y * bitmap_w + x];
      glyph->image.image().fast_set(x, y, Pal8::get_gray(pix), {});
      glyph->image.mask().fast_set(x, y, Pal8::mask_visible, {});
    }
  }
  free(bitmap);
  return true;
} // _load_glyph
