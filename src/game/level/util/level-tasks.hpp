#pragma once
#include <deque>
#include <functional>
#include "util/math/num-types.hpp"

using Level_task = std::function<bool (const Delta_time dt)>;
using Level_tasks = std::deque<Level_task>;

void execute_tasks(Level_tasks& tasks, Delta_time dt);
