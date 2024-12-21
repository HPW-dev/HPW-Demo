#pragma once
#include "window.hpp"
#include "util/mem-types.hpp"

class Entity_editor_ctx;
class Scene_entity_editor;

// оконо для спавнеров
class Window_emitter final: public Window {
public:
  explicit Window_emitter(Scene_entity_editor& master, Entity_editor_ctx& ctx);
  ~Window_emitter();
  void imgui_exec() override;
  void update(const Delta_time dt) override;
  inline bool active() const { return m_active; }

private:
  bool m_active {true}; // false - выключит окно

  nocopy(Window_emitter);
  struct Impl;
  Unique<Impl> impl {};
};

