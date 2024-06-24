#pragma once
struct Pal8;
struct Rgb24;

using to_palette_rgb24_ft = Rgb24 (*)(const Pal8 x);

Rgb24 to_palette_rgb24_default(const Pal8 x);

// преобразует палитровый цвет в RGB24 с учётом текущей палитры
inline to_palette_rgb24_ft to_palette_rgb24 = &to_palette_rgb24_default;
