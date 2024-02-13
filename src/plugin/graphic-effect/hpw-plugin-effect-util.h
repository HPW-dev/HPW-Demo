#pragma once
#include "hpw-plugin-effect.h"

struct rgb24_t pal8_to_rgb24(const pal8_t src);
pal8_t rgb24_to_pal8(const struct rgb24_t src);
bool check_params(const struct context_t* context, struct result_t* result);