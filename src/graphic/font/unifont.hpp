#pragma once
#include <map>
#include "font.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/file/file.hpp"

struct stbtt_fontinfo;

struct Glyph;
class Sprite;

// шрифт для unifont.ttf
class Unifont: public hpw::Font {
  bool mono_ {};
  mutable std::map<char32_t, Shared<Glyph>> glyph_table {}; // кэш символов
  real scale_ {};
  Shared<stbtt_fontinfo> info_ {};
  Bytes font_file_mem_ {};

  // возвращает картинку символа. если её нет в кэше, то добавляет туда
  cp<Glyph> _get_glyph(char32_t ch) const;
  virtual Shared<Glyph> _load_glyph(char32_t ch) const; // грузит новый шрифт. ret loaded Glyph if success

public:
  // @param mono enable black/white color
  explicit Unifont(cr<Str> fname, int height=12, bool mono=true);
  // @param mono enable black/white color
  explicit Unifont(cr<File> file, int height=12, bool mono=true);
  ~Unifont() = default;
  // узнать ширину текста
  int text_width(cr<utf32> text) const override;
  // отобразить текст
  void draw(Image& dst, const Vec pos, cr<utf32> text,
    blend_pf bf=&blend_diff, const int optional=0) const override;
}; // Unifont
