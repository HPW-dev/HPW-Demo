#pragma once
#include "window.hpp"
#include "util/mem-types.hpp"
#include "util/macro.hpp"

class Entity_editor_ctx;

// верхнее меню в редакторе игровых объектов
class Wnd_ent_edit_opts final: public Window {
  nocopy(Wnd_ent_edit_opts);
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Wnd_ent_edit_opts(Entity_editor_ctx& ctx);
  ~Wnd_ent_edit_opts();
  inline void draw(Image& dst) const {}
  inline void update(const Delta_time dt) {}
  void imgui_exec() override;
};
