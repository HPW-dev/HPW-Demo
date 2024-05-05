#pragma once
#include <stack>
#include <functional>
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "util/mem-types.hpp"

class Scene;
class Image;

// управление сценами
class Scene_mgr final {
  std::stack<Shared<Scene>> scene_stack {};

  using Job = std::function<void ()>;
  /** @brief для отложенной вставки и смены сцен.
  Повышает безопасность вызова add и back в любом месте */ 
  Vector<Job> jobs {};
  
  void _add(CN<Shared<Scene>> scene);
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
  bool update(double dt);
  void draw(Image& dst) const;
  Scene* get_current() const;
  void add(CN<Shared<Scene>> scene); // добавить новую сцену
  void back(); // вернуться в предыдущую сцену
}; // Scene_mgr
