#include <imgui.h>
#include <algorithm>
#include <iomanip>
#include "menu.hpp"
#include "window-global.hpp"
#include "game/core/core.hpp"
#include "game/core/common.hpp"
#include "game/core/scenes.hpp"
#include "game/scene/scene-mgr.hpp"
#include "game/util/sync.hpp"
#include "game/util/keybits.hpp"
#include "game/util/game-util.hpp"
#include "util/log.hpp"
#include "util/hpw-util.hpp"
#include "util/str-util.hpp"
#include "util/file/yaml.hpp"
#include "graphic/animation/anim-io.hpp"
#include "scene/yn.hpp"

inline void save_all_anims() {
  Yaml file (hpw::cur_dir + "../config/animation.yml");
  save_anims(file);
}

void Menu_wnd::update(const Delta_time dt) {
  using namespace ImGui;
  
  if (is_pressed_once(hpw::keycode::escape))
    back_from_wnd();
  if (IsKeyReleased(ImGuiKey_Space))
    editor::is_pause = !editor::is_pause;
  if (IsKeyDown(ImGuiKey_LeftCtrl) && IsKeyReleased(ImGuiKey_S))
    save_all_anims();
  if (IsKeyDown(ImGuiKey_LeftCtrl) && IsKeyReleased(ImGuiKey_R))
    editor::is_reset = true;
} // update

void Menu_wnd::imgui_exec() {
  using namespace ImGui;
  BeginMainMenuBar();
  Scope _({}, &EndMainMenuBar);
  
  // менюшка сверху слева
  if (BeginMenu("Menu")) {
    Scope _({}, &EndMenu);
    if (MenuItem("Pause", "SPACE"))
      editor::is_pause = !editor::is_pause;
    if (MenuItem("Save", "CTRL+S"))
      save_all_anims();
    if (MenuItem("Reload", "CTRL+R"))
      editor::is_reset = true;
    if (MenuItem("Exit", "ESC") || IsKeyDown(ImGuiKey_Escape))
      back_from_wnd();
  } // BeginMenu
  
  // инфу о таймингах в меню
  std::stringstream ss;
  #define prec(val) std::fixed << std::setprecision(val) << \
  std::setw(std::max(6, val)) << std::setfill(' ')
  ss << "FPS " << prec(1) << graphic::cur_fps << " | ";
  ss << "UPS " << prec(1) << hpw::cur_ups << " | ";
  ss << "R.Dt " << prec(8) << hpw::real_dt << " | ";
  ss << "S.Dt " << prec(8) << hpw::safe_dt << " | ";
  ss << "Draw " << prec(8) << graphic::soft_draw_time << " | ";
  ss << "Update " << prec(8) << hpw::tick_time;
  #undef prec
  BeginMenu(ss.str().c_str(), false);
} // imgui_exec

void Menu_wnd::back_from_wnd() {
  hpw::scene_mgr->add(new_shared<Yes_no_scene>(
    u8"Выйти из редактора?",
    []{ hpw::scene_mgr->back(); } ));
}
