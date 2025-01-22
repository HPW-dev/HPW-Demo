#pragma once
#include <stack>
#include <functional>
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"

class Scene;
class Image;

// управление сценами
class Scene_mgr final {
  std::stack<Shared<Scene>> scene_stack {};

  using Job = std::function<void ()>;
  /** @brief для отложенной вставки и смены сцен.
  Повышает безопасность вызова add и back в любом месте */ 
  Vector<Job> jobs {};
  
  void _add(cr<Shared<Scene>> scene);
  void _back();
  void lazy_scene_update();

public:
  struct Status {
    bool came_back: 1 {}; // было возвращение из сцены
    bool next_scene: 1 {}; // был переход в новую сцену
    bool empty: 1 {}; // сцены кончились
  };
  mutable Status status {};

  Scene_mgr() = default;
  ~Scene_mgr() = default;
  // @return false если сцены кончились
  [[nodiscard]] bool update(const Delta_time dt);
  void draw(Image& dst) const;
  Scene* get_current() const;
  void add(cr<Shared<Scene>> scene); // добавить новую сцену
  void back(uint count=1); // вернуться обратно по стеку сцен <count> раз
}; // Scene_mgr
