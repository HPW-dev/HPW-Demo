#pragma once
#include <functional>
#include "util/math/num-types.hpp"
#include "util/vector-types.hpp"
#include "util/macro.hpp"

class Image;

struct Task {
  using Func_update = std::function<void (Task&, const Delta_time dt)>;
  using Func_draw = std::function<void (CN<Task>, Image& dst)>;
  using Action = std::function<void (Task&)>;
  bool active {true};
  bool paused {};
  
  Func_update update_f {};
  Func_draw draw_f {};
  Action on_start {}; // действие при запуске
  Action on_end {}; // действие при завершении
  Action on_stop {}; // действие при стопе
  Action on_unfreeze {}; // действие при выходе из стопа
};

void stop(Task& task);
void unfreeze(Task& task);
void kill(Task& task);
void restart(Task& task);

// рулит задачами
class Task_mgr final {
  using Tasks = Vector<Task>;
  Tasks m_tasks {};

  void process_killed();

public:
  nocopy(Task_mgr);
  Task_mgr() = default;
  ~Task_mgr() = default;

  Task& add(CN<Task> task);
  Task& move(Task&& task);
  void update(const Delta_time dt);
  void draw(Image& dst) const;
  void reset_all();
  void stop_all();
  void unfreeze_all();
  void kill_all();
};
