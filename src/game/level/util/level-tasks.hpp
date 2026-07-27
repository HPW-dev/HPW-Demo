#pragma once
#include <deque>
#include <functional>
#include "util/math/num-types.hpp"
#include "util/vector-types.hpp"
#include "util/macro.hpp"

using Level_task = std::function<bool (const Delta_time dt)>;
using Level_tasks = std::deque<Level_task>;
using Level_task_complete_cb = std::function<void (const Delta_time dt)>;
using Level_task_complete_cbs = Vector<Level_task_complete_cb>;

void execute_tasks(Level_tasks& tasks, cr<Level_task_complete_cbs> complete_cbs, Delta_time dt);
