#pragma once
#include <algorithm>
#include "util/macro.hpp"
#include "util/math/num-types.hpp"

struct Rgb24 final {
  using value_t = byte;
  value_t r {}, g {}, b {};
  
  Rgb24() noexcept = default;
  Rgb24(int ir, int ig, int ib) noexcept;

  struct Null_tag final {}; // чтобы указать на constexpr c-tor
  inline constexpr Rgb24(byte _r, byte _g, byte _b, const Null_tag tag) noexcept
    : r{_r}, g{_g}, b{_b} {}
  
  inline static constexpr Rgb24 neutral() { return {}; }
};

/** Индексированные цвета HPW
*@details Диапазоны: 0 B, 0..222 BW, 223..254 Red, 255 W. */
struct Pal8 final {
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

  Pal8() noexcept = default;
  Pal8(cr<Pal8>) noexcept = default;
  inline constexpr Pal8(value_t nval) noexcept: val(nval) {}
  Pal8& operator = (cr<Pal8>) noexcept = default;
  Pal8& operator = (value_t nval) noexcept;
  bool operator == (auto other) const noexcept { return val == Pal8(other).val; }
  bool operator != (auto other) const noexcept { return val ^ Pal8(other).val; }
  inline constexpr void set(int nval) noexcept { val = scast<byte>(nval); }
  void set_gray(int value) noexcept;
  inline constexpr void set_red(int value) noexcept;
  void set_red_nosafe(int value) noexcept;
  static Pal8 get_gray(int value) noexcept;
  inline constexpr static Pal8 get_red(int value) noexcept;
  inline constexpr static Pal8 neutral() noexcept { return Pal8{Pal8::none}; }
  static Pal8 get_red_nosafe(int value) noexcept;
  bool is_red() const noexcept;
  bool is_white() const noexcept;
  void add(int x) noexcept;
  void sub(int x) noexcept;
  void mul(int x) noexcept;
  void inc() noexcept;
  void dec() noexcept;
  void apply_invert() noexcept;
  template <class T> operator T() const noexcept { return T(val); }
  Pal8& operator += (int x) noexcept;
  Pal8& operator -= (int x) noexcept;
  Pal8& operator *= (int x) noexcept;
  Pal8& operator +=(const Pal8 x) noexcept;
  Pal8& operator -=(const Pal8 x) noexcept;
  Pal8& operator *=(const Pal8 x) noexcept;
  Pal8& operator ++() noexcept;
  Pal8& operator --() noexcept;
  Pal8 operator +(int x) const noexcept;
  Pal8 operator -(int x) const noexcept;
  Pal8 operator *(int x) const noexcept;
  bool operator <(int x) const noexcept;
  Pal8 operator +(const Pal8 x) const noexcept;
  Pal8 operator -(const Pal8 x) const noexcept;
  Pal8 operator *(const Pal8 x) const noexcept;
  Pal8 operator ~() const noexcept;
  real to_real() const noexcept; // преобразование цвета в число 0..1
  
  // создаёт цвет из числа в диапазоне 0..1
  inline static constexpr Pal8 from_real(real src, bool is_red = false) noexcept;
}; // Pal8

inline constexpr Pal8 Pal8::from_real(real src, bool is_red) noexcept {
  if (src >= 1.0)
    return is_red ? red : white;
  if (src <= 0.0)
    return is_red ? red_start : black;
  return is_red ?
    get_red(src * red_size) :
    Pal8(src * gray_size);
} // from_real

inline constexpr Pal8 Pal8::get_red(int value) noexcept {
  Pal8 ret;
  ret.set_red(value);
  return ret;
}

inline constexpr void Pal8::set_red(int value) noexcept
  { val = std::min(uint(value) + uint(red_start), uint(red)); }
