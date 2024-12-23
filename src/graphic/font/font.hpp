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
  virtual int text_width(cr<utf32> text) const noexcept;
  // узнать высоту текста
  virtual int text_height(cr<utf32> text) const noexcept;
  // размер текста в 2D векторе
  virtual Vec text_size(cr<utf32> text) const noexcept;
  // отобразить текст
  virtual void draw(Image& dst, const Vec pos, cr<utf32> text,
    blend_pf bf = &blend_past, const int optional = 0) const = 0;
  inline crauto w() const noexcept { return w_; }
  inline crauto h() const noexcept { return h_; }
  inline crauto space() const noexcept { return space_; }
}; // Font
