#include <imgui.h>
#include "scene-editor-select.hpp"
#include "scene-entity-editor.hpp"
#include "scene-anim-editor.hpp"
#include "yn.hpp"
#include "entity-editor-ctx.hpp"
#include "graphic/image/image.hpp"
#include "game/core/canvas.hpp"
#include "game/core/scenes.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"

struct Scene_editor_select::Impl {
  Unique<Advanced_text_menu> m_menu {};

  inline Impl() {
    init_unique( m_menu,
      U"Редакторы",
      Menu_items {
        new_shared<Menu_text_item>(U"Редактор анимаций и хитбоксов", [this]{ goto_anim_editor(); }),
        new_shared<Menu_text_item>(U"Редактор игровых объектов", [this]{ goto_entity_editor(); }),
        new_shared<Menu_text_item>(U"Выход", [this]{ exit(); })
      },
      Rect(0,0, graphic::canvas->X, graphic::canvas->Y)
    );
  }

  inline void update(const Delta_time dt) {
    m_menu->update(dt);
  }

  inline void draw(Image& dst) const {
    m_menu->draw(dst);
  }

  inline void imgui_exec() {
    if (ImGui::IsKeyDown(ImGuiKey_Escape))
      exit();
  }

  inline void exit() { hpw::scene_mgr->back(); }
  inline void goto_anim_editor() { hpw::scene_mgr->add(new_shared<Scene_anim_editor>()); }
  inline void goto_entity_editor() { hpw::scene_mgr->add(new_shared<Scene_entity_editor>()); }
}; // Impl

Scene_editor_select::Scene_editor_select(): impl {new_unique<Impl>()} {}
Scene_editor_select::~Scene_editor_select() {}
void Scene_editor_select::update(const Delta_time dt) { impl->update(dt); }
void Scene_editor_select::draw(Image& dst) const { impl->draw(dst); }
void Scene_editor_select::imgui_exec() { impl->imgui_exec(); }
