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

// EXPORTED - делает функцию видной из .dll/.so
// NOT_EXPORTED - скрывает функцию в .dll/.so
#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define EXPORTED __attribute__ ((dllexport))
  #else
    #define EXPORTED __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
  #endif
  #define NOT_EXPORTED
#else
  #if __GNUC__ >= 4
    #define EXPORTED __attribute__ ((visibility ("default")))
    #define NOT_EXPORTED  __attribute__ ((visibility ("hidden")))
  #else
    #define EXPORTED
    #define NOT_EXPORTED
  #endif
#endif

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define PLUG_CALL __attribute__ ((__cdecl__))
  #else
    #define PLUG_CALL __cdecl
  #endif
#else
  #define PLUG_CALL
#endif

#define DEFAULT_EFFECT_API_VERSION 2

typedef uint8_t pal8_t;
typedef const char* cstr_t;
typedef float real_t;

// name, description, value ref, speed step, min, max
typedef void (*registrate_param_f32_ft)(cstr_t, cstr_t, real_t*,
  const real_t, const real_t, const real_t);
// name, description, value ref, speed step, min, max
typedef void (*registrate_param_i32_ft)(cstr_t, cstr_t, int32_t*,
  const int32_t, const int32_t, const int32_t);
// name, description, value ref
typedef void (*registrate_param_bool_ft)(cstr_t, cstr_t, bool*);

struct rgb24_t {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

// Передаёт данные в эффект
struct context_t {
  pal8_t* dst;
  uint16_t w;
  uint16_t h;
  registrate_param_f32_ft registrate_param_f32;
  registrate_param_i32_ft registrate_param_i32;
  registrate_param_bool_ft registrate_param_bool;
};

// для получения данных с эффекта
struct result_t {
  uint8_t version;
  cstr_t full_name;
  cstr_t author;
  cstr_t description;
  cstr_t error;
  bool init_succsess;
};

EXPORTED void PLUG_CALL plugin_init(const struct context_t* context, struct result_t* result);
EXPORTED void PLUG_CALL plugin_apply(uint32_t state);
EXPORTED void PLUG_CALL plugin_finalize(void);

#ifdef __cplusplus
}
#endif
