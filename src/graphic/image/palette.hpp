#pragma once
#include "util/macro.hpp"
#include "util/str.hpp"
#include "util/vector-types.hpp"

struct Pal8;
struct Rgb24;
struct File;

Rgb24 to_palette_rgb24_default(const Pal8 x);

using to_palette_rgb24_ft = Rgb24 (*)(const Pal8 x);
// преобразует палитровый цвет в RGB24 с учётом текущей палитры
inline to_palette_rgb24_ft to_palette_rgb24 = &to_palette_rgb24_default;

// список цветов в картинке с палитрой
Vector<Rgb24> colors_from_pal24(cr<File> src);
