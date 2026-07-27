#pragma once
#include "window.hpp"
#include "util/mem-types.hpp"
#include "util/macro.hpp"

class Scene_entity_editor;

// верхнее меню в редакторе игровых объектов
class Wnd_ent_edit_menu final: public Window {
  nocopy(Wnd_ent_edit_menu);
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Wnd_ent_edit_menu(Scene_entity_editor& master);
  ~Wnd_ent_edit_menu();
  inline void draw(Image& dst) const {}
  inline void update(const Delta_time dt) {}
  void imgui_exec() override;
};
