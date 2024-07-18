#include <cassert>
#include <imgui.h>
#include "scene-entity-editor.hpp"
#include "yn.hpp"
#include "window.hpp"
#include "entity-window/wnd-ent-edit-menu.hpp"
#include "entity-editor-ctx.hpp"
#include "graphic/image/image.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "util/log.hpp"

struct Scene_entity_editor::Impl {
  Scene_entity_editor* m_master {};
  Entity_editor_ctx m_ctx {}; // данные редактора распределяемые между окнами
  Vector<Unique<Window>> m_windows {};

  explicit inline Impl(Scene_entity_editor* master)
  : m_master {master} {
    assert(m_master);
    m_windows.push_back(new_unique<Wnd_ent_edit_menu>(m_master));
  }

  inline void update(const Delta_time dt) {
    for (cnauto window: m_windows)
      window->update(dt);
  }

  inline void draw(Image& dst) const {
    draw_bg(dst);
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

  inline void draw_bg(Image& dst) const {
    graphic::canvas->fill(Pal8::gray);
  }

  inline void pause() {
    // TODO
    hpw_log("need impl");
  }

  inline void save() {
    // TODO
    hpw_log("need impl");
  }

  inline void reload() {
    // TODO
    hpw_log("need impl");
  }
}; // Impl

Scene_entity_editor::Scene_entity_editor(): impl {new_unique<Impl>(this)} {}
Scene_entity_editor::~Scene_entity_editor() {}
void Scene_entity_editor::update(const Delta_time dt) { impl->update(dt); }
void Scene_entity_editor::draw(Image& dst) const { impl->draw(dst); }
void Scene_entity_editor::imgui_exec() { impl->imgui_exec(); }
void Scene_entity_editor::exit() { impl->exit(); }
void Scene_entity_editor::pause() { impl->pause(); }
void Scene_entity_editor::save() { impl->save(); }
void Scene_entity_editor::reload() { impl->reload(); }
