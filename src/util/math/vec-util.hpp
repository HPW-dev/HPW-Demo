#pragma once
#include <cmath>
#include "mat.hpp"
#include "util/macro.hpp"
#include "util/math/vec.hpp"
#include "util/str.hpp"

Vec rotate_rad(const Vec center, const Vec src, real radian);
Vec rotate_deg(const Vec center, const Vec src, real degree);
real length(const Vec src);
Vec normalize_stable(const Vec src);
Vec normalize_graphic(const Vec src);
[[nodiscard]] Vec floor(const Vec src) noexcept;
[[nodiscard]] Vec ceil(const Vec src) noexcept;
[[nodiscard]] Vec abs(const Vec src) noexcept;
// угол между вектором в радианах
real rad_between_vecs(const Vec a, const Vec b);
// угол между вектором
real deg_between_vecs(const Vec a, const Vec b);
// получить случайную точку вокруг pos в радиусе r (синхронный)
Vec make_rand_by_radius_stable(const Vec pos, real r);
// получить случайную точку вокруг pos в радиусе r
Vec make_rand_by_radius_graphic(const Vec pos, real r);

Str to_str(const Vec src);
Str to_str(const Vec src, real prec);

// угол по единичному вектору
real vec_to_deg(Vec vec);

// случайный угол для без защиты рандома
real rand_degree_graphic();
// случайный угол для c защитой рандома
real rand_degree_stable();

// случайный единичный вектор без защиты рандома
Vec rand_normalized_graphic();
// случайный единичный вектор c защитой рандома
Vec rand_normalized_stable();

real distance(const Vec a, const Vec b);
real fast_distance(const Vec a, const Vec b);

// угол между двумя векторами направления
[[nodiscard]] real deg_between_vecs(const Vec a, const Vec b);

// единичный вектор направления по углу
template <typename T>
[[nodiscard]] inline Vec_base<T> deg_to_vec(const T deg)
  { return { std::cos(deg_to_rad(deg)), std::sin(deg_to_rad(deg)) }; }
