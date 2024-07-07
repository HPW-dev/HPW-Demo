#include <algorithm>
#include <utility>
#include "task.hpp"
#include "util/safecall.hpp"
#include "util/error.hpp"

Task::~Task() { 
  if (m_active)
    kill();
}

void Task::stop() {
  m_paused = true;
  on_stop();
}

void Task::unfreeze() {
  m_paused = false;
  on_unfreeze();
}

void Task::kill() {
  if (m_active) {
    m_active = false;
    on_end();
  }
}

void Task::restart() {
  if (m_active)
    on_end();
  if (!m_active)    
    on_start();
}

void Task_mgr::process_killed() {
  // удалить всех неактивных
  std::erase_if(m_tasks, [](CN<Shared<Task>> task)
    { return !task || !task->is_active(); });
}

Shared<Task> Task_mgr::add(CN<Shared<Task>> task) {
  iferror(!task, "bad task ptr");
  m_tasks.push_back(task);
  task->on_start();
  return task;
}

Shared<Task> Task_mgr::move(Shared<Task>&& task) {
  iferror(!task, "bad task ptr");
  nauto ret = m_tasks.emplace_back( std::move(task) );
  ret->on_start();
  return ret;
}

void Task_mgr::update(const Delta_time dt) {
  process_killed();

  for (nauto task: m_tasks) {
    cont_if(!task);
    cont_if(!task->is_active());
    cont_if(task->is_paused());
    task->update(dt);
  }
}

void Task_mgr::draw(Image& dst) const {
  for (cnauto task: m_tasks)
    if (task->is_active())
      task->draw(dst);
}

void Task_mgr::reset_all() {
  for (nauto task: m_tasks)
    task->restart();
}

void Task_mgr::stop_all() {
  for (nauto task: m_tasks)
    task->stop();
}

void Task_mgr::unfreeze_all() {
  for (nauto task: m_tasks)
    task->unfreeze();
}

void Task_mgr::kill_all() { m_tasks.clear(); }

Task_mgr::~Task_mgr() { clear(); }

void Task_mgr::clear() {
  for (nauto task: m_tasks)
    task->deactivate();
  m_tasks.clear();
}
