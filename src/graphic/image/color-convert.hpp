#pragma once
#include "util/macro.hpp"
#include "util/math/num-types.hpp"

struct HSL;
struct Pal8;
struct Rgb24;
struct Rgbr;

Rgb24 to_rgb24(const Pal8 x);
Pal8 desaturate_luma(int R, int G, int B);
Pal8 desaturate_average(int R, int G, int B);
HSL pal8_to_hsl(const Pal8 src);
HSL rgb24_to_hsl(const Rgb24 src);
Rgbr rgb24_to_real(cr<Rgb24> src);
real to_hue(cr<Rgbr> src);
Rgb24 to_rgb24(int ir, int ig, int ib);
Rgbr lerp(cr<Rgbr> a, cr<Rgbr> b, real t);
real to_luma(cr<Rgbr> col);

void operator +=(Rgbr& dst, cr<Rgbr> src);
void operator -=(Rgbr& dst, cr<Rgbr> src);
void operator *=(Rgbr& dst, cr<Rgbr> src);
void operator /=(Rgbr& dst, cr<Rgbr> src);

void operator +=(Rgbr& dst, real src);
void operator -=(Rgbr& dst, real src);
void operator *=(Rgbr& dst, real src);
void operator /=(Rgbr& dst, real src);

Rgbr operator + (cr<Rgbr> a, cr<Rgbr> b);
Rgbr operator - (cr<Rgbr> a, cr<Rgbr> b);
Rgbr operator * (cr<Rgbr> a, cr<Rgbr> b);
Rgbr operator / (cr<Rgbr> a, cr<Rgbr> b);

Rgbr operator + (cr<Rgbr> a, real b);
Rgbr operator - (cr<Rgbr> a, real b);
Rgbr operator * (cr<Rgbr> a, real b);
Rgbr operator / (cr<Rgbr> a, real b);
