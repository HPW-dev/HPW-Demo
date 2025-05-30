#pragma once
#include "util/unicode.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/color-blend.hpp"

class Image;

// базовый класс для шрифтов
class Font_base {
  nocopy(Font_base);
  
protected:
  int w_ = 0; // ширина шрифта
  int h_ = 0; // высота шрифта
  Veci space_ {}; // величина отступа

public:
  Font_base() = default;
  virtual ~Font_base() = default;
  // узнать ширину текста
  virtual int text_width(cr<utf32> text) const noexcept;
  // узнать высоту текста
  virtual int text_height(cr<utf32> text) const noexcept;
  // размер текста в 2D векторе
  virtual Veci text_size(cr<utf32> text) const noexcept;
  // отобразить текст
  virtual void draw(Image& dst, const Veci pos, cr<utf32> text,
    blend_pf bf = &blend_past, const int optional = 0) const = 0;
  virtual bool is_mono() const = 0;
  inline crauto w() const noexcept { return w_; }
  inline crauto h() const noexcept { return h_; }
  inline crauto space() const noexcept { return space_; }
  void set_w(int w);
  void set_h(int h);
  void set_space(const Veci space);
}; // Font_base
