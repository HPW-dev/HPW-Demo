#include <math.h>
#include "hpw-plugin-effect-util.h"

struct rgb24_t pal8_to_rgb24(const pal8_t src) {
  struct rgb24_t ret;
  if (src == 255u) {
    ret.r = ret.g = ret.b = 0xFFu;
    return ret;
  }
  if ( !(src > 222 && src <= 254) ) {
    const double GRAY_MUL = 255.0 / 222.0;
    ret.r = ret.g = ret.b = round(src * GRAY_MUL);
    return ret;
  }

  ret.g = ret.b = 0;
  const double RED_MUL = 255.0 / 32.0;
  ret.r = round((src - 222) * RED_MUL);
  return ret;
}

pal8_t rgb24_to_pal8(const struct rgb24_t src) {
  // если R оттенок сильнее G и B, то это красный цвет с палитры
  if (src.r > 10 + ((src.g + src.b) >> 1)) {
    const double RED_MUL = 32.0 / 255.0;
    return 222 + round(src.r * RED_MUL);
  } else if ((src.g & src.b & src.r & 0xFFu) == 0xFFu) { // полностью белый
    return 255u; // white
  }

  // всё остальное, это серый
  const double GRAY_MUL = 222.0 / 255.0;
  const double gray = src.r * 0.299 + src.g * 0.587 + src.b * 0.114;
  return round(gray * GRAY_MUL);
}

bool check_params(const struct context_t* context, struct result_t* result) {
  result->error = "";
  result->version = DEFAULT_EFFECT_API_VERSION;
  result->init_succsess = true;

  #define iferror(cond, msg) if (cond) { \
    result->error = msg; \
    result->init_succsess = false; \
    return false; \
  }
  iferror( !context->dst, "context.dst is null");
  iferror(context->h == 0, "context.h is 0");
  iferror(context->w == 0, "context.w is 0");
  iferror( !context->registrate_param_f32, "registrate_param_f32 is null");
  iferror( !context->registrate_param_i32, "registrate_param_i32 is null");
  iferror( !context->registrate_param_bool, "registrate_param_bool is null");
  #undef iferror
  return true;
}
