#include <math.h>
#include "hpw-plugin-effect-util.h"

NOT_EXPORTED
struct rgb24_t pal8_to_rgb24(const pal8_t src) {
  struct rgb24_t ret;
  if (src == 255u) {
    ret.r = ret.g = ret.b = 0xFFu;
    return ret;
  }
  if ( !(src > 222 && src <= 254) ) {
    const real GRAY_MUL = 255.0f / 222.0f;
    ret.r = ret.g = ret.b = round(src * GRAY_MUL);
    return ret;
  }

  ret.g = ret.b = 0;
  const real RED_MUL = 255.0f / 32.0f;
  ret.r = round((src - 222) * RED_MUL);
  return ret;
}

NOT_EXPORTED
pal8_t rgb24_to_pal8(const struct rgb24_t src) {
  // если R оттенок сильнее G и B, то это красный цвет с палитры
  if (src.r > 10 + ((src.g + src.b) >> 1)) {
    const real RED_MUL = 32.0f / 255.0f;
    return 222 + round(src.r * RED_MUL);
  } else if ((src.g & src.b & src.r & 0xFFu) == 0xFFu) { // полностью белый
    return 255u; // white
  }

  // всё остальное, это серый
  const real GRAY_MUL = 222.0f / 255.0f;
  const real gray = src.r * 0.299f + src.g * 0.587f + src.b * 0.114f;
  return round(gray * GRAY_MUL);
}

NOT_EXPORTED
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
