extern "C" {
#define STB_TRUETYPE_IMPLEMENTATION 
#include <stb/stb_truetype.h>
}
#include <algorithm>
#include <sstream>
#include <memory>
#include "unifont.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/str-util.hpp"
#include "util/file/file-io.hpp"

Unifont::Unifont(cr<Str> fname, int height, bool mono)
: Unifont(file_load(fname), height, mono) {}

Unifont::Unifont(cr<File> file, int height, bool mono) {
  init(file, height, mono);
}

void Unifont::init(cr<File> file, int height, bool mono) {
  log_debug << "loading font \"" + file.get_path() + "\"";
  mono_ = mono;
  font_file_mem_ = file.data;
  h_ = height;
  space_ = {1, 1};
  init_shared(info_);
  iferror( !stbtt_InitFont(info_.get(), font_file_mem_.data(), 0),
    "Unifont: stbtt_InitFont error");
  scale_ = stbtt_ScaleForPixelHeight(info_.get(), height);
  // предзагрузка ASCI; 33 - символ после пробела (!):
  for (int i = 33; i < 128; ++i)
    _load_glyph(i);
}

int Unifont::text_width(cr<utf32> text) const noexcept {
  int size = 0;
  int max_size = 0;

  for (auto ch: text) {
    if (ch == U'\n') {
      max_size = std::max(size, max_size);
      size = 0;
    }

    const int glyph_w = Font_base::w_ == 0 ? _get_glyph(ch)->image.X() : Font_base::w_;
    size += glyph_w + space_.x;
  }

  max_size = std::max(size, max_size);
  return max_size + 1;
}

void Unifont::draw(Image& dst, const Veci pos, cr<utf32> text,
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
    // \r возврат каретки
    if (ch == '\r') {
      posx = pos.x;
      continue;
    }

    auto glyph = _get_glyph(ch);
    continue_if( !glyph);
    insert(dst, glyph->image, {posx + glyph->xoff, posy + glyph->yoff}, bf, optional);
    posx += glyph->image.X() + space().x;
  } // for text size
} // draw

cp<Unifont::Glyph> Unifont::_get_glyph(char32_t ch) const {
  // найти символ в кэше
  try {
    return glyph_table_.at(ch).get();
  }
  // если нет символа, загрузить или вернуть null
  catch (...) {
    return_if (!_load_glyph(ch), nullptr);
    return glyph_table_.at(ch).get();
  }
  return {};
} // _get_glyph

Shared<Unifont::Glyph> Unifont::_load_glyph(char32_t ch) const {
  // пробел - просто путая картинка
  if (ch == U' ') {
    int ax, lsb;
    stbtt_GetCodepointHMetrics(info_.get(), ' ', &ax, &lsb);
    init_shared(glyph_table_[ch]);
    auto& glyph = glyph_table_.at(ch);
    glyph->image.init(ax * scale_, 1);
    glyph->image.image().fill(Pal8::black);
    glyph->yoff = -1;
    return glyph;
  }

  int bitmap_w, bitmap_h, bitmap_xoff, bitmap_yoff;
  auto bitmap = stbtt_GetCodepointBitmap(info_.get(), scale_, scale_, ch,
    &bitmap_w, &bitmap_h, &bitmap_xoff, &bitmap_yoff);
  if ( !bitmap) {
    std::stringstream ss;
    ss << "stbtt_GetCodepointBitmap error (" << std::hex << int(ch) << ")\n";
    log_debug << ss.str();
    return {};
  }

  init_shared(glyph_table_[ch]);
  auto& glyph = glyph_table_.at(ch);
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
  return glyph;
} // _load_glyph
