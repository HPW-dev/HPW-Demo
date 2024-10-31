#pragma once
#include <map>
#include "font.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/file/file.hpp"
#include "graphic/sprite/sprite.hpp"

struct stbtt_fontinfo;

// шрифт для unifont.ttf
class Unifont: public hpw::Font {
public:
  struct Glyph {
    Sprite image {};
    int xoff {}, yoff {};
  };

  // @param mono enable black/white color
  explicit Unifont(cr<Str> fname, int height=12, bool mono=true);
  // @param mono enable black/white color
  explicit Unifont(cr<File> file, int height=12, bool mono=true);
  ~Unifont() = default;
  // узнать ширину текста
  int text_width(cr<utf32> text) const noexcept override final;
  // отобразить текст
  void draw(Image& dst, const Vec pos, cr<utf32> text,
    blend_pf bf=&blend_diff, const int optional=0) const override final;
  
private:
  bool mono_ {};
  mutable std::map<char32_t, Shared<Glyph>> glyph_table_ {}; // кэш символов
  real scale_ {};
  Shared<stbtt_fontinfo> info_ {};
  Bytes font_file_mem_ {};

  // возвращает картинку символа. если её нет в кэше, то добавляет туда
  cp<Glyph> _get_glyph(char32_t ch) const;

protected:
  Unifont() = default;
  void init(cr<File> file, int height, bool mono);
  // грузит новый шрифт. ret loaded Glyph if success
  virtual Shared<Unifont::Glyph> _load_glyph(char32_t ch) const;
}; // Unifont
