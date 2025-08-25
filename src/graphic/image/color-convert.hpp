#pragma once
struct HSL;
struct Pal8;
struct Rgb24;

HSL pal8_to_hsl(const Pal8 src);
HSL rgb24_to_hsl(const Rgb24 src);
