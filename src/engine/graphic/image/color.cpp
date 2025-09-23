#include "color.hpp"

#ifdef ECOMEM
#define NOUSE_TABLE
#endif

#ifndef NOUSE_TABLE
#include "color-table.hpp"
#endif

#ifndef NOUSE_TABLE
void Pal8::add(int x)     noexcept { val = table_add_safe[std::clamp(x, 0, 255)*256 + val]; }
void Pal8::sub(int x)     noexcept { val = table_sub_safe[std::clamp(x, 0, 255)*256 + val]; }
void Pal8::mul(int x)     noexcept { val = table_mul_safe[std::clamp(x, 0, 255)*256 + val]; }
void Pal8::inc()          noexcept { val = table_inc_safe[val]; }
void Pal8::dec()          noexcept { val = table_dec_safe[val]; }
void Pal8::apply_invert() noexcept { val = table_inv_safe[val]; }
#else
void Pal8::add(int x)     noexcept { val = std::clamp<int>(scast<int>(val) + x, 0, 255); }
void Pal8::sub(int x)     noexcept { val = std::clamp<int>(scast<int>(val) - x, 0, 255); }
void Pal8::mul(int x)     noexcept { val = std::clamp<int>(scast<int>(val) * x, 0, 255); }
void Pal8::inc()          noexcept { val = std::clamp<int>(scast<int>(val) + 1, 0, 255); }
void Pal8::dec()          noexcept { val = std::clamp<int>(scast<int>(val) - 1, 0, 255); }
void Pal8::apply_invert() noexcept { val = ~val; }
#endif

real Pal8::to_real() const noexcept {
  if (is_red()) {
    constexpr real red_mul = 1.0 / red_size;
    return (val - red_start) * red_mul;
  } elif (is_white()) {
    return 1.0;
  }
  // gray:
  constexpr real gray_mul = 1.0 / gray_size;
  return val * gray_mul;
} // to_real

Pal8& Pal8::operator = (value_t nval) noexcept { val = nval; return *this; }

bool Pal8::is_red() const noexcept { return val > gray_end && val < white; }
bool Pal8::is_white() const noexcept { return val == gray_end || val == white; }
Pal8& Pal8::operator += (int x) noexcept { add(x); return *this; }
Pal8& Pal8::operator -= (int x) noexcept { sub(x); return *this; }
Pal8& Pal8::operator *= (int x) noexcept { mul(x); return *this; }
Pal8& Pal8::operator +=(const Pal8 x) noexcept { add(x.val); return *this; }
Pal8& Pal8::operator -=(const Pal8 x) noexcept { sub(x.val); return *this; }
Pal8& Pal8::operator *=(const Pal8 x) noexcept { mul(x.val); return *this; }
Pal8& Pal8::operator ++() noexcept { inc(); return *this; }
Pal8& Pal8::operator --() noexcept { dec(); return *this; }
Pal8 Pal8::operator +(int x) const noexcept { Pal8 ret(*this); ret.add(x); return ret; }
Pal8 Pal8::operator -(int x) const noexcept { Pal8 ret(*this); ret.sub(x); return ret; }
Pal8 Pal8::operator *(int x) const noexcept { Pal8 ret(*this); ret.mul(x); return ret; }
Pal8 Pal8::operator +(const Pal8 x) const noexcept { Pal8 ret(*this); ret.add(x.val); return ret; }
Pal8 Pal8::operator -(const Pal8 x) const noexcept { Pal8 ret(*this); ret.sub(x.val); return ret; }
Pal8 Pal8::operator *(const Pal8 x) const noexcept { Pal8 ret(*this); ret.mul(x.val); return ret; }
Pal8 Pal8::operator ~() const noexcept { Pal8 ret(*this); ret.apply_invert(); return ret; }
bool Pal8::operator <(int x) const noexcept { return val < x; }

Pal8 Pal8::get_gray(int value) noexcept {
  Pal8 ret;
  ret.set_gray(value);
  return ret;
}

Pal8 Pal8::get_red_nosafe(int value) noexcept {
  Pal8 ret;
  ret.set_red_nosafe(value);
  return ret;
}

void Pal8::set_gray(int value) noexcept
  { val = std::clamp<int>(value, black, gray_end); }

void Pal8::set_red_nosafe(int value) noexcept
  { val = scast<int>(red_start) + (value % scast<int>(red_size)); }

Rgb24::Rgb24(int ir, int ig, int ib) noexcept
: r (std::clamp(ir, 0, 255))
, g (std::clamp(ig, 0, 255))
, b (std::clamp(ib, 0, 255))
{}
