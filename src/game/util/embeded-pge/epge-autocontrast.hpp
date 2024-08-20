#pragma once
#include "util/macro.hpp"
#include "plugin/graphic-effect/hpw-plugin-effect.h"

namespace Epge_autocontrast {
extern "C" void plugin_init(cp<context_t> context, result_t* result);
extern "C" void plugin_apply(uint32_t state);
extern "C" void plugin_finalize();
}
