#include "scene-anim-editor.hpp"
#include "anim-window/root.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/util/keybits.hpp"
#include "game/scene/scene-mgr.hpp"
#include "game/core/entities.hpp"
#include "game/core/debug.hpp"
#include "yn.hpp"

Scene_anim_editor::Scene_anim_editor()
: root(new_shared<Root_wnd>())
{}

void Scene_anim_editor::update(const Delta_time dt) {
  root->update(dt);
}

void Scene_anim_editor::draw(Image& dst) const {
  root->draw(dst);
  post_draw(dst);
}

void Scene_anim_editor::post_draw(Image& dst) const {
  if (graphic::draw_hitboxes) { // показать хитбоксы
    insert<&blend_no_black>(dst, *hpw::hitbox_layer, {});
    hpw::hitbox_layer->fill(Pal8::black);
  }

  if (graphic::show_grids) // сетки системы коллизий
    hpw::entity_mgr->debug_draw(dst);
} 

void Scene_anim_editor::imgui_exec() {
  root->imgui_exec();
}
