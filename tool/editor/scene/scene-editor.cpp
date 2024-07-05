#include "scene-editor.hpp"
#include "window/root.hpp"
#include "graphic/image/image.hpp"
#include "game/util/keybits.hpp"
#include "game/scene/scene-mgr.hpp"
#include "yn.hpp"

Editor_scene::Editor_scene()
: root(new_shared<Root_wnd>())
{}

void Editor_scene::update(const Delta_time dt) {
  root->update(dt);
}

void Editor_scene::draw(Image& dst) const {
  root->draw(dst);
}

void Editor_scene::imgui_exec() {
  root->imgui_exec();
}
