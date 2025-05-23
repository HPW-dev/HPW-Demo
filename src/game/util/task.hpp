#pragma once
#include <utility>
#include "util/math/num-types.hpp"
#include "util/vector-types.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"

class Image;

// Базовый класс для задач в геймлупе
class Task {
  bool m_active {true};
  bool m_paused {};

public:
  virtual ~Task();
  inline virtual void on_start() {}
  inline virtual void on_end() {}
  inline virtual void on_stop() {}
  inline virtual void on_unfreeze() {}
  inline virtual void update(const Delta_time dt) {}
  inline virtual void draw(Image& dst) const {} // рисоватьповерх всей сцены
  inline virtual void draw_post_bg(Image& dst) const {} // рисовать поверх фона уровня
  virtual void stop();
  virtual void unfreeze();
  virtual void kill();
  virtual void restart();
  inline bool is_active() const { return m_active; }
  inline bool is_paused() const { return m_paused; }
  // сигнал на завершение задачи (не вызывай это, если хочешь убить объект, вызывай kill())
  inline void deactivate() { m_active = false; }
  inline void activate() { m_active = true; }
};

// рулит задачами
class Task_mgr final {
  using Tasks = Vector<Shared<Task>>;
  Tasks m_tasks {};

  void process_killed();

public:
  nocopy(Task_mgr);
  Task_mgr() = default;
  ~Task_mgr();

  Shared<Task> add(cr<Shared<Task>> task);
  Shared<Task> add(Shared<Task>&& task);
  void update(const Delta_time dt);
  void draw(Image& dst) const; // рендер поверх всей сцены
  void draw_post_bg(Image& dst) const; // рендер после отрисовки фона
  void reset_all();
  void stop_all();
  void unfreeze_all();
  void kill_all();
  void clear();
};
