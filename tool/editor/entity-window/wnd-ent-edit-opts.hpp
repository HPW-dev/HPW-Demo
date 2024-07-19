#pragma once
#include "window.hpp"
#include "util/mem-types.hpp"
#include "util/macro.hpp"

class Scene_entity_editor;

// верхнее меню в редакторе игровых объектов
class Wnd_ent_edit_opts final: public Window {
  nocopy(Wnd_ent_edit_opts);
  struct Impl;
  Unique<Impl> impl {};

public:
  Wnd_ent_edit_opts();
  ~Wnd_ent_edit_opts();
  inline void draw(Image& dst) const {}
  inline void update(const Delta_time dt) {}
  void imgui_exec() override;
};
