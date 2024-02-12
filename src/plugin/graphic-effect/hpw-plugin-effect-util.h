#pragma once
#include "hpw-plugin-effect.h"

struct Rgb24 pal8_to_rgb24(const Pal8 src);
Pal8 rgb24_to_pal8(const struct Rgb24 src);
void check_params(const struct Context* context, struct Result* result);