#pragma once
#include <deque>
#include <functional>

using Level_task = std::function<bool (double dt)>;
using Level_tasks = std::deque<Level_task>;

void execute_tasks(Level_tasks& tasks, double dt);
