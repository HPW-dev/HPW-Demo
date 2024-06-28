#include <algorithm>
#include <utility>
#include "task.hpp"
#include "util/safecall.hpp"

void Task_mgr::process_killed() {
  // удалить всех неактивных
  std::erase_if(m_tasks, [](CN<Task> task) { return !task.active; });
}

Task& Task_mgr::add(CN<Task> task) {
  m_tasks.push_back(task);
  return m_tasks.back();
}

Task& Task_mgr::move(Task&& task) {
  return m_tasks.emplace_back(std::move(task));;
}

void Task_mgr::update(const Delta_time dt) {
  for (nauto task: m_tasks) {
    if (task.active && !task.paused && task.update_f)
      task.update_f(task, dt);
  }
}

void Task_mgr::draw(Image& dst) const {
  for (cnauto task: m_tasks) {
    if (task.active && task.draw_f)
      task.draw_f(task, dst);
  }
}

void Task_mgr::reset_all() {
  for (nauto task: m_tasks) {
    safecall(task.on_end, task);
    safecall(task.on_start, task);
  }
}

void Task_mgr::stop_all() {
  for (nauto task: m_tasks) {
    safecall(task.on_stop, task);
    task.paused = true;
  }
}

void Task_mgr::unfreeze_all() {
  for (nauto task: m_tasks) {
    safecall(task.on_unfreeze, task);
    task.paused = false;
  }
}

void Task_mgr::kill_all() {
  for (nauto task: m_tasks)
    task.active = false;
}
