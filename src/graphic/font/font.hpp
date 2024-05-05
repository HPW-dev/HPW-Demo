#pragma once
#include "util/unicode.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/color-blend.hpp"

class Image;

// фикс алиаса имён из X11
namespace hpw { class Font; }

// базовый класс для шрифтов
class hpw::Font {
  nocopy(Font);
  
protected:
  int w_ = 0; // ширина шрифта
  int h_ = 0; // высота шрифта
  Vec space_ {}; // величина отступа

public:
  Font() = default;
  virtual ~Font() = default;
  // узнать ширину текста
  virtual int text_width(CN<utf32> text) const;
  // узнать высоту текста
  virtual int text_height(CN<utf32> text) const;
  // размер текста в 2D векторе
  virtual Vec text_size(CN<utf32> text) const;
  // отобразить текст
  virtual void draw(Image& dst, const Vec pos, CN<utf32> text,
    blend_pf bf = &blend_past, const int optional = 0) const = 0;
  inline cnauto w() const { return w_; }
  inline cnauto h() const { return h_; }
  inline cnauto space() const { return space_; }
}; // Font
