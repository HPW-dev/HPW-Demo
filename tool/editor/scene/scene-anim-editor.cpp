#include "scene-anim-editor.hpp"
#include "window/root.hpp"
#include "graphic/image/image.hpp"
#include "game/util/keybits.hpp"
#include "game/scene/scene-mgr.hpp"
#include "yn.hpp"

Scene_anim_editor::Scene_anim_editor()
: root(new_shared<Root_wnd>())
{}

void Scene_anim_editor::update(const Delta_time dt) {
  root->update(dt);
}

void Scene_anim_editor::draw(Image& dst) const {
  root->draw(dst);
}

void Scene_anim_editor::imgui_exec() {
  root->imgui_exec();
}
