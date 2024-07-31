#include <sstream>
#include <iomanip>
#include <imgui.h>
#include "wnd-ent-edit-menu.hpp"
#include "util/hpw-util.hpp"
#include "scene/scene-entity-editor.hpp"
#include "game/util/sync.hpp"
#include "game/core/core.hpp"

struct Wnd_ent_edit_menu::Impl {
  Scene_entity_editor& m_master;

  inline explicit Impl(Scene_entity_editor& master): m_master{master} {}

  inline void imgui_exec() {
    ImGui::BeginMainMenuBar();
    Scope _({}, &ImGui::EndMainMenuBar);

    if (ImGui::BeginMenu("меню")) {
      Scope _({}, &ImGui::EndMenu);
      if (ImGui::MenuItem("добавить эмиттер", "CTRL+A"))
        m_master.add_emitter();
      if (ImGui::MenuItem("пауза", "SPACE"))
        m_master.pause();
      if (ImGui::MenuItem("сохранить", "CTRL+S"))
        m_master.save();
      if (ImGui::MenuItem("перезагрузить", "CTRL+R"))
        m_master.reload();
      if (ImGui::MenuItem("выйти", "ESC"))
        m_master.exit();
    }

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
    ImGui::BeginMenu(ss.str().c_str(), false);

    process_imgui_input();
  }

  // обработка горячих кдавишь
  inline void process_imgui_input() {
    using namespace ImGui;
    if (IsKeyDown(ImGuiKey_LeftCtrl) && IsKeyReleased(ImGuiKey_A))
      m_master.add_emitter();
    if (IsKeyDown(ImGuiKey_LeftCtrl) && IsKeyReleased(ImGuiKey_S))
      m_master.save();
    if (IsKeyDown(ImGuiKey_LeftCtrl) && IsKeyReleased(ImGuiKey_R))
      m_master.reload();
    if (IsKeyReleased(ImGuiKey_Space))
      m_master.pause();
  }
}; // Impl

Wnd_ent_edit_menu::Wnd_ent_edit_menu(Scene_entity_editor& master): impl {new_unique<Impl>(master)} {}
Wnd_ent_edit_menu::~Wnd_ent_edit_menu() {}
void Wnd_ent_edit_menu::imgui_exec() { impl->imgui_exec(); }
