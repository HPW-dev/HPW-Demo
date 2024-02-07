#include <imgui.h>
#include "opts.hpp"
#include "game/game-graphic.hpp"
#include "game/game-sync.hpp"
#include "game/game-debug.hpp"
#include "host/command.hpp"
#include "util/hpw-util.hpp"
#include "window-global.hpp"
#include "util/str-util.hpp"

void Opts_wnd::imgui_exec() {
  using namespace ImGui;
  Begin("Options", {}, ImGuiWindowFlags_AlwaysAutoResize);
  Scope _({}, &ImGui::End);
  Checkbox("pause", &editor::is_pause);
  // фулскрин
  bool is_fullscreen = graphic::fullscreen;
  if (Checkbox("fullscreen", &is_fullscreen)) {
    graphic::fullscreen = is_fullscreen;
    hpw::set_fullscreen(graphic::fullscreen);
  }
  // v-sync
  bool is_vsync = graphic::get_vsync();
  if (Checkbox("VSync", &is_vsync))
    graphic::set_vsync(is_vsync);
  Checkbox("cross", &editor::use_draw_cross);
  Checkbox("zoom x2", &editor::use_zoom_x2);
  Checkbox("draw frames", &editor::use_draw_frames);
  SameLine();
  int mode = scast<int>(editor::draw_frames_mode);
  scauto sprite_draw_modes = strs_to_null_terminated(Strs {
    "sprite",
    "mask",
    "image"
  });
  PushItemWidth(100);
  Combo("##sprite draw mode", &mode, sprite_draw_modes.data());
  PopItemWidth();
  editor::draw_frames_mode = Draw_frames_mode(mode);
  Checkbox("bg color red", &editor::bg_color_red);
  Text("bg color val");
  SliderFloat("##bg color val", &editor::bg_color_val, 0, 1);
  Checkbox("other bg", &editor::other_bg);
  
  if (
    bool draw_hitboxes = graphic::draw_hitboxes;
    Checkbox("draw hitboxes", &draw_hitboxes))
  { graphic::draw_hitboxes = draw_hitboxes; }
} // imgui_exec
