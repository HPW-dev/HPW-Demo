#pragma once
#include "hpw-plugin-effect.h"

NOT_EXPORTED struct rgb24_t pal8_to_rgb24(const pal8_t src);
NOT_EXPORTED pal8_t rgb24_to_pal8(const struct rgb24_t src);
NOT_EXPORTED bool check_params(const struct context_t* context, struct result_t* result);