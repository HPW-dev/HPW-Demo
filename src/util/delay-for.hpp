#pragma once
#include <functional>
#include "util/math/num-types.hpp"

using Cond_checker = std::function<bool ()>;

/** ожидает пока cond_checker вернёт true с таймаутом (timeout_sec)
 * @return true, если выход не по таймауту, а по условию cond_checker */
bool wait_for(Cond_checker cond_checker, Delta_time timeout_sec = 10, bool use_yield = true);
