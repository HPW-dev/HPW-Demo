#include <imgui.h>
#include "scene-entity-editor.hpp"
#include "yn.hpp"
#include "window.hpp"
#include "entity-editor-ctx.hpp"
#include "graphic/image/image.hpp"
#include "game/core/scenes.hpp"

struct Scene_entity_editor::Impl {
  Entity_editor_ctx m_ctx {}; // данные редактора распределяемые между окнами
  Vector<Unique<Window>> m_windows {};

  inline void update(const Delta_time dt) {
    for (cnauto window: m_windows)
      window->update(dt);
  }

  inline void draw(Image& dst) const {
    for (cnauto window: m_windows)
      window->draw(dst);
  }

  inline void imgui_exec() {
    if (ImGui::IsKeyDown(ImGuiKey_Escape))
      exit();
    for (cnauto window: m_windows)
      window->imgui_exec();
  }

  inline void exit() {
    hpw::scene_mgr->add( new_shared<Yes_no_scene>(
      u8"Выйти из редактора?", []{ hpw::scene_mgr->back(); } ) );
  }
}; // Impl

Scene_entity_editor::Scene_entity_editor(): impl {new_unique<Impl>()} {}
Scene_entity_editor::~Scene_entity_editor() {}
void Scene_entity_editor::update(const Delta_time dt) { impl->update(dt); }
void Scene_entity_editor::draw(Image& dst) const { impl->draw(dst); }
void Scene_entity_editor::imgui_exec() { impl->imgui_exec(); }
