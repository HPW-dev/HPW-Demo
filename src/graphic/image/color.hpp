#pragma once
#include "util/macro.hpp"
#include "util/math/num-types.hpp"

struct Rgb24 {
  struct Null_tag {}; // чтобы указать на constexpr c-tor

  using value_t = byte;
  value_t r {}, g {}, b {};

  Rgb24() = default;
  Rgb24(int ir, int ig, int ib);
  inline constexpr Rgb24(byte _r, byte _g, byte _b, const Null_tag tag): r{_r}, g{_g}, b{_b} {}
};

/** Индексированные цвета HPW
*@details Диапазоны: 0 B, 0..222 BW, 223..254 Red, 255 W. */
struct Pal8 {
  using value_t = byte;
  constx value_t black {0};
  constx value_t gray_end {221}; // WHITE2
  constx value_t red_start {222};
  constx value_t red {254}; // red end
  constx value_t white {255}; // WHITE1
  constx value_t red_black {red_start};
  constx value_t red_end {red};
  constx value_t red_mid {red_start + (red_end - red_start) / 2};
  constx value_t red_size {scast<int>(red_end) - scast<int>(red_start)};
  constx value_t gray_size {scast<int>(gray_end) - scast<int>(black) + 1};
  constx value_t gray {scast<int>(gray_end) / 2};
  constx value_t mask_visible {black};
  constx value_t mask_invisible {white};
  constx value_t none {mask_invisible}; // for image get bound func
  value_t val {black};

  Pal8() = default;
  Pal8(CN<Pal8>) = default;
  inline constexpr Pal8(value_t nval) noexcept: val(nval) {}
  Pal8& operator = (CN<Pal8>) = default;
  Pal8& operator = (value_t nval);
  bool operator == (auto other) const { return val == Pal8(other).val; }
  bool operator != (auto other) const { return val ^ Pal8(other).val; }
  inline constexpr void set(int nval) { val = scast<byte>(nval); }
  void set_gray(int value);
  inline constexpr void set_red(int value);
  void set_red_nosafe(int value);
  static Pal8 get_gray(int value);
  inline constexpr static Pal8 get_red(int value);
  static Pal8 get_red_nosafe(int value);
  bool is_red() const;
  bool is_white() const;
  void add(int x);
  void sub(int x);
  void mul(int x);
  void inc();
  void dec();
  void apply_invert();
  template <class T> operator T() const { return T(val); }
  Pal8& operator += (int x);
  Pal8& operator -= (int x);
  Pal8& operator *= (int x);
  Pal8& operator +=(const Pal8 x);
  Pal8& operator -=(const Pal8 x);
  Pal8& operator *=(const Pal8 x);
  Pal8& operator ++();
  Pal8& operator --();
  Pal8 operator +(int x) const;
  Pal8 operator -(int x) const;
  Pal8 operator *(int x) const;
  bool operator <(int x) const;
  Pal8 operator +(const Pal8 x) const;
  Pal8 operator -(const Pal8 x) const;
  Pal8 operator *(const Pal8 x) const;
  Pal8 operator ~() const;
  real to_real() const; // преобразование цвета в число 0..1
  
  // создаёт цвет из числа в диапазоне 0..1
  inline static constexpr Pal8 from_real(real src, bool is_red = false);
}; // Pal8

inline constexpr Pal8 Pal8::from_real(real src, bool is_red) {
  if (src >= 1.0)
    return is_red ? red : white;
  if (src <= 0.0)
    return is_red ? red_start : black;
  return is_red ?
    get_red(src * red_size) :
    Pal8(src * gray_size);
} // from_real

inline constexpr Pal8 Pal8::get_red(int value) {
  Pal8 ret;
  ret.set_red(value);
  return ret;
}

inline constexpr void Pal8::set_red(int value)
  { val = std::min(uint(value) + uint(red_start), uint(red)); }
