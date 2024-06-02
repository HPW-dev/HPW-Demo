#include "level-tasks.hpp"
#include "util/macro.hpp"

void execute_tasks(Level_tasks& tasks, Delta_time dt) {
  return_if(tasks.empty());

  // выполнить задачу, если true, то её можно выкинуть из списка
  if (bool complete = tasks.front()(dt); complete)
    tasks.pop_front();
}
