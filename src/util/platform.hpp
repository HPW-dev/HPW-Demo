#pragma once
#include "util/str.hpp"
#include "util/macro.hpp"

// разделители файловых систем:
#ifdef WINDOWS
  #define SEPARATOR '\\'
  #define NSEPARATOR '/'
// LINUX
#else
  #define SEPARATOR '/'
  #define NSEPARATOR '\\'
#endif

// узнать какая разрядность:
#ifdef __i386__
  #define is_x32 
#else
  #define is_x64
#endif

#if defined(__clang__)
  #define COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
  #define COMPILER_GCC
#elif defined(_MSC_VER)
  #define COMPILER_MSC
  #pragma GCC diagnostic warning "not tested on MSC!"
#endif

using Seconds = double;

enum class Priority {
  low,
  normal,
  high,
  realtime,
};

// подождать секунды
void delay_sec(const Seconds seconds);

// подгонка задержки для повышения точности
void calibrate_delay(const Seconds target);

// выбрать таймер по имени
void set_timer(cr<Str> name);

// унать какой таймер используется сейчас
Str get_timer();

// устанавливает приоритет процесса
void set_priority(Priority mode);

// время в секундах после старта программы
Seconds get_cur_time();

// как в системе называется компьютер
Str get_pc_name();
