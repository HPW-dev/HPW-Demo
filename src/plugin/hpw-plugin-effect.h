#pragma once
/**
* @file API для графических плагинов HPW совместимых с SEZEIII
* @version 1
* @date 11.02.2024 */

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#define DEFAULT_EFFECT_API_VERSION 1

typedef uint8_t Pal8;
typedef const char* Cstr;

/// name, description, value ref, speed step, min, max
typedef void (*registrate_param_f32_ft)(Cstr, Cstr, float*, const float, const float, const float);
/// name, description, value ref, speed step, min, max
typedef void (*registrate_param_i32_ft)(Cstr, Cstr, int*, const int, const int, const int);

struct Rgb24 {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

/// Передаёт данные в эффект
struct Context {
  Pal8* dst;
  uint16_t w;
  uint16_t h;
  registrate_param_f32_ft registrate_param_f32;
  registrate_param_i32_ft registrate_param_i32;
};

/// для получения данных с эффекта
struct Result {
  uint8_t version;
  Cstr full_name;
  Cstr description;
  Cstr error;
  bool init_succsess;
};

void init(const struct Context* context, struct Result* result);
void apply(uint32_t state);
void finalize(void);

#ifdef __cplusplus
}
#endif
