#pragma once
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"
#include "graphic/image/image-fwd.hpp"

class Scene;

// управление сценами
class Scene_mgr final {
  nocopy(Scene_mgr);

public:
  struct Status {
    bool came_back: 1 {}; // было возвращение из сцены
    bool next_scene: 1 {}; // был переход в новую сцену
    bool empty: 1 {true}; // сцены кончились
  };

  Scene_mgr();
  ~Scene_mgr();
  void add(cr<Shared<Scene>> scene); // добавить новую сцену
  void add(cr<Strs> name); // добавить новую сцену по названию
  void draw(Image& dst) const;
  void back(uint count=1); // вернуться обратно по стеку сцен <count> раз
  void clear();
  [[nodiscard]] bool update(Delta_time dt); // @return false если сцены кончились
  [[nodiscard]] cr<Status> status() const;
  [[nodiscard]] Shared<Scene> current() const;
  [[nodiscard]] Strs names() const; // имена имеющихся сцен

private:
  struct Impl;
  Unique<Impl> _impl {};
};
