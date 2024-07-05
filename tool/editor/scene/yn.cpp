#include <imgui.h>
#include "yn.hpp"
#include "util/hpw-util.hpp"
#include "game/scene/scene-mgr.hpp"
#include "game/util/keybits.hpp"
#include "game/core/scenes.hpp"
#include "editor-util.hpp"

Yes_no_scene::Yes_no_scene(CN<utf8> msg,
CN<decltype(callback)> new_callback)
: msg_(msg), callback(new_callback) {}

void Yes_no_scene::update(const Delta_time dt) {
  using namespace ImGui;
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr->back();
  if (is_pressed_once(hpw::keycode::enable))
    enable();
}

void Yes_no_scene::imgui_exec() {
  using namespace ImGui;
  Begin(u8tos(u8"Внимание"), {},
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_AlwaysAutoResize);
  Scope _({}, &ImGui::End);
  Text(u8tos(msg_.c_str()));
  if (Button(u8tos(u8"Да")))
    enable();
  SameLine();
  if (Button(u8tos(u8"Нет")))
    hpw::scene_mgr->back();
} // imgui_execute

void Yes_no_scene::enable() {
  hpw::scene_mgr->back();
  callback();
}
