#include "level-tasks.hpp"
#include "util/macro.hpp"

void execute_tasks(Level_tasks& tasks, cr<Level_task_complete_cbs> complete_cbs, Delta_time dt) {
  return_if(tasks.empty());

  // выполнить задачу, если true, то её можно выкинуть из списка
  if (bool complete = tasks.front()(dt); complete) {
    for (rauto cb: complete_cbs)
      cb(dt);
    tasks.pop_front();
  }
}
