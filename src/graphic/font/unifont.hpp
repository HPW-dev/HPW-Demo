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
class Unifont final: public hpw::Font {
  bool mono_ {};
  mutable std::map<char32_t, Shared<Glyph>> glyph_table {}; // кэш символов
  real scale_ {};
  Shared<stbtt_fontinfo> info_ {};
  Bytes font_file_mem_ {};

  // возвращает картинку символа. если её нет в кэше, то добавляет туда
  CP<Glyph> _get_glyph(char32_t ch) const;
  bool _load_glyph(char32_t ch) const; // грузит новый шрифт. ret 1 if success

public:
  explicit Unifont(CN<Str> fname, int height=12, bool mono=true);
  explicit Unifont(CN<File> file, int height=12, bool mono=true);
  ~Unifont() = default;
  // узнать ширину текста
  int text_width(CN<utf32> text) const override;
  // отобразить текст
  void draw(Image& dst, const Vec pos, CN<utf32> text,
    blend_pf bf=&blend_diff, const int optional=0) const override;
}; // Unifont
