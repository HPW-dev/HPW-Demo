#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include "pge-util.hpp"
#include "graphic/image/color.hpp"

bool check_params(const context_t* context, result_t* result) {
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
} // check_params

Pal8& get_pixel_fast(Pal8 image[], const int x, const int y, const int width)
{ return image[x + width * y]; }

void set_pixel_fast(Pal8 image[], const int x, const int y, const int width,
const Pal8 val)
  { image[x + width * y] = val; }
