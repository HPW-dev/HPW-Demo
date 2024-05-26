#include <imgui.h>
#include "input.hpp"
#include "util/hpw-util.hpp"
#include "game/core/scenes.hpp"
#include "game/util/keybits.hpp"
#include "game/scene/scene-manager.hpp"

Input_scene::Input_scene(CN<Str> new_title, CN<callback_t> new_callback)
: title{new_title}, callback{new_callback}
{ charbuf[0] = '\0'; }

void Input_scene::update(double dt) {
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr->back();
  if (is_pressed_once(hpw::keycode::enable)) {
    hpw::scene_mgr->back();
    auto return_str {from_null_ended<Str>(charbuf)};
    callback(return_str);
  }
}

void Input_scene::imgui_exec() {
  ImGui::Begin("Input", {},
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_AlwaysAutoResize);
  Scope _({}, &ImGui::End);
  ImGui::InputText(title.c_str(), charbuf.data(), charbuf.size());
  if (ImGui::Button("accept"))  {
    hpw::scene_mgr->back();
    auto return_str {from_null_ended<Str>(charbuf)};
    callback(return_str);
  }
  ImGui::SameLine();
  if (ImGui::Button("back"))
    hpw::scene_mgr->back();
}
