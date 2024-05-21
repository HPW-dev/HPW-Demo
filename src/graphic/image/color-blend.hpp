#pragma once
//@file пиксельблендинг операции
#include "color.hpp"
#include "color-table.hpp"

/** blend func type.
* @param bg фоновой пиксель
* @param in накладываемый пиксель */
using blend_pf = Pal8 (*)(const Pal8 in, const Pal8 bg, int optional);

#ifndef ECOMEM

[[nodiscard, gnu::const]] inline Pal8 blend_inv      (const Pal8 bg, int optional=0) { return table_inv[uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_inv_safe (const Pal8 bg, int optional=0) { return table_inv_safe[uint(bg.val)]; }
[[nodiscard, gnu::const]] Pal8 blend_rotate          (const Pal8 in, const Pal8 bg, int optional=0);
[[nodiscard, gnu::const]] Pal8 blend_rotate_x4       (const Pal8 in, const Pal8 bg, int optional=0);
[[nodiscard, gnu::const]] Pal8 blend_rotate_x16      (const Pal8 in, const Pal8 bg, int optional=0);
[[nodiscard, gnu::const]] Pal8 blend_rotate_safe     (const Pal8 in, const Pal8 bg, int optional=0);
[[nodiscard, gnu::const]] Pal8 blend_rotate_x4_safe  (const Pal8 in, const Pal8 bg, int optional=0);
[[nodiscard, gnu::const]] Pal8 blend_rotate_x16_safe (const Pal8 in, const Pal8 bg, int optional=0);
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_none(const Pal8 in, const Pal8 bg, int optional=0) { return bg; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_past(const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline Pal8 blend_fade_in_max(const Pal8 in, const Pal8 bg, int optional) { return table_fade_in_max[uint(in.val) + uint(bg.val)*256 + scast<uint>(scast<byte>(optional))*256*256]; }
[[nodiscard, gnu::const]] inline Pal8 blend_fade_out_max(const Pal8 in, const Pal8 bg, int optional) { return table_fade_out_max[uint(in.val) + uint(bg.val)*256 + scast<uint>(scast<byte>(optional))*256*256]; }
[[nodiscard, gnu::const]] inline Pal8 blend_or       (const Pal8 in, const Pal8 bg, int optional=0) { return table_or       [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_sub      (const Pal8 in, const Pal8 bg, int optional=0) { return table_sub      [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_add      (const Pal8 in, const Pal8 bg, int optional=0) { return table_add      [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_mul      (const Pal8 in, const Pal8 bg, int optional=0) { return table_mul      [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_and      (const Pal8 in, const Pal8 bg, int optional=0) { return table_and      [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_min      (const Pal8 in, const Pal8 bg, int optional=0) { return table_min      [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_max      (const Pal8 in, const Pal8 bg, int optional=0) { return table_max      [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_avr      (const Pal8 in, const Pal8 bg, int optional=0) { return table_avr      [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_avr_max  (const Pal8 in, const Pal8 bg, int optional=0) { return table_avr_max  [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_158      (const Pal8 in, const Pal8 bg, int optional=0) { return table_blend158 [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_diff     (const Pal8 in, const Pal8 bg, int optional=0) { return table_diff     [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_xor      (const Pal8 in, const Pal8 bg, int optional=0) { return table_xor      [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_xor_safe (const Pal8 in, const Pal8 bg, int optional=0) { return table_xor_safe [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_overlay  (const Pal8 in, const Pal8 bg, int optional=0) { return table_overlay  [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_or_safe  (const Pal8 in, const Pal8 bg, int optional=0) { return table_or_safe  [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_add_safe (const Pal8 in, const Pal8 bg, int optional=0) { return table_add_safe [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_sub_safe (const Pal8 in, const Pal8 bg, int optional=0) { return table_sub_safe [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_mul_safe (const Pal8 in, const Pal8 bg, int optional=0) { return table_mul_safe [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_and_safe (const Pal8 in, const Pal8 bg, int optional=0) { return table_and_safe [uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_softlight(const Pal8 in, const Pal8 bg, int optional=0) { return table_softlight[uint(in.val)*256 + uint(bg.val)]; }
[[nodiscard, gnu::const]] inline Pal8 blend_no_black (const Pal8 in, const Pal8 bg, int optional=0) { return in == Pal8::black ? bg : in; }
[[nodiscard, gnu::const]] inline Pal8 blend_diff_no_black (const Pal8 in, const Pal8 bg, int optional=0) { return in == Pal8::black ? bg : blend_diff(in, bg, optional); }
// @param optional это прозрачность (255 - непрозрачен, 0 - полностью прозрачен)
[[nodiscard, gnu::const]] inline Pal8 blend_alpha(const Pal8 in, const Pal8 bg, int optional) { return table_blend_alpha[uint(in.val) + uint(bg.val)*256 + scast<uint>(scast<byte>(optional))*256*256]; }

#else

#pragma message("ECOMEM pixel blending is not compile")
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_inv             (const Pal8 bg, int optional=0) { return ~bg.val; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_inv_safe        (const Pal8 bg, int optional=0) { return ~bg.val; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_rotate          (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_rotate_x4       (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_rotate_x16      (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_rotate_safe     (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_rotate_x4_safe  (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_rotate_x16_safe (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_none(const Pal8 in, const Pal8 bg, int optional=0) { return bg; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_past(const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_fade_in_max(const Pal8 in, const Pal8 bg, int optional) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_fade_out_max(const Pal8 in, const Pal8 bg, int optional) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_or       (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_sub      (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_add      (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_mul      (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_and      (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_min      (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_max      (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_avr      (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_avr_max  (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_158      (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_diff     (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_xor      (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_xor_safe (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_overlay  (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_or_safe  (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_add_safe (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_sub_safe (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_mul_safe (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_and_safe (const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_softlight(const Pal8 in, const Pal8 bg, int optional=0) { return in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_no_black (const Pal8 in, const Pal8 bg, int optional=0) { return in == Pal8::black ? bg : in; }
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_diff_no_black (const Pal8 in, const Pal8 bg, int optional=0) { return in == Pal8::black ? bg : in; }
// @param optional это прозрачность (255 - непрозрачен, 0 - полностью прозрачен)
[[nodiscard, gnu::const]] inline constexpr Pal8 blend_alpha(const Pal8 in, const Pal8 bg, int optional) { return in; }

#endif
