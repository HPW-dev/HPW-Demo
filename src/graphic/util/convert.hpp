#pragma once
#include "util/macro.hpp"

struct Rgb24;
struct Pal8;

Rgb24 to_rgb24(const Pal8 x);
Pal8 desaturate_bt601(int R, int G, int B);
Pal8 desaturate_average(int R, int G, int B);
