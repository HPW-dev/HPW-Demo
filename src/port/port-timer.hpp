#pragma once
#include "util/str.hpp"
#include "util/macro.hpp"


using Seconds = double;

// подождать секунды
void delay_sec(const Seconds seconds);
// подгонка задержки для повышения точности
void calibrate_delay(const Seconds target);
// выбрать таймер по имени
void set_timer(cr<Str> name);
// унать какой таймер используется сейчас
Str get_timer();
// время в секундах после старта программы
Seconds get_cur_time();
