#include <imgui.h>
#include "window-emitter.hpp"
#include "entity-editor-ctx.hpp"
#include "util/hpw-util.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "game/entity/entity.hpp"
#include "game/core/entities.hpp"
#include "game/util/game-util.hpp"

namespace {
Uid g_wnd_uid {}; // чтобы ImGui не перепутал окна
}

struct Window_emitter::Impl {
  Entity_editor_ctx& m_ctx;
  Window_emitter& m_master;
  Uid m_uid {};
  Str m_title {}; // название окна для ImGui
  float m_deg {};
  float m_speed {};
  float m_force {};
  float m_accel {};
  float m_rot_frc {};
  float m_rot_acc {};
  Vec m_pos {};
  Entity* m_last_entity {};

  std::size_t m_selected_name_id {};
  Str m_name_for_spawn {}; // выбранный entity

  inline explicit Impl(Window_emitter& master, Entity_editor_ctx& ctx)
  : m_ctx {ctx}
  , m_master {master}
  , m_uid {get_uid()}
  {
    m_title = "Эмиттер " + n2s(m_uid);
    detailed_log("добавлено окно эмиттера (№" << m_uid << ")\n");
    m_pos = get_screen_center();
  }

  inline void imgui_exec() {
    ImGui::Begin(m_title.c_str(), {}, ImGuiWindowFlags_AlwaysAutoResize);
    Scope _({}, &ImGui::End);
    name_select();
    phys_select();
    imgui_input();
    if (ImGui::Button("Закрыть"))
      exit();
  }

  inline static Uid get_uid() { return ::g_wnd_uid++; }

  // выбирает доступный для спавна объект
  inline void name_select() {
    Strs names = m_ctx.entities_yml.root_tags();
    if (names.empty())
      names.push_back("entities.yml не содержит имён объектов");

    if (ImGui::BeginCombo("объект", names.at(m_selected_name_id).c_str())) {
      for (uint i; cnauto name: names) {
        const bool is_selected = (i == m_selected_name_id);
        if (ImGui::Selectable(name.c_str(), is_selected)) {
          m_selected_name_id = i;
          kill_cur_entity();
        }
        if (is_selected)
          ImGui::SetItemDefaultFocus();
        ++i;
      }
      ImGui::EndCombo();
    } // if BeginCombo

    m_name_for_spawn = names.at(m_selected_name_id);
  }

  inline void imgui_input() {
    // сочетание клавишь на выход
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_W))
      exit();
  }

  inline void exit() { m_master.m_active = false; }

  // настраивает физику спавнера
  inline void phys_select() {
    ImGui::Separator();
    float pos[2] {m_pos.x, m_pos.y};
    if (ImGui::SliderFloat2("позиция", pos, -50, 600)) {
      m_pos.x = pos[0];
      m_pos.y = pos[1];
    }
    ImGui::SliderFloat("направление", &m_deg, 0, 360);
    ImGui::SliderFloat("скорость", &m_speed, 0, 150_pps);
    ImGui::SliderFloat("ускорение", &m_accel, 0, 150_pps);
    ImGui::SliderFloat("торможение", &m_force, 0, 150_pps);
    ImGui::SliderFloat("ускор. вращ.", &m_rot_frc, 0, 720);
    ImGui::SliderFloat("тормо. вращ.", &m_rot_acc, 0, 150_pps);
  } // phys_select

  inline void update(const Delta_time dt) {
    return_if(m_name_for_spawn.empty());
    return_if(m_last_entity && m_last_entity->status.live);

    m_last_entity = hpw::entity_mgr->make({}, m_name_for_spawn, m_pos);
    assert(m_last_entity);
    m_last_entity->phys.set_pos(m_pos);
    m_last_entity->phys.set_deg(m_deg);
    m_last_entity->phys.set_speed(m_speed);
    m_last_entity->phys.set_force(m_force);
    m_last_entity->phys.set_accel(m_accel);
    m_last_entity->phys.set_rot_ac(m_rot_acc);
    m_last_entity->phys.set_rot_fc(m_rot_frc);
  }

  inline void kill_cur_entity() {
    return_if(!m_last_entity || !m_last_entity->status.live);
    m_last_entity->kill();
  }
}; // Impl

Window_emitter::Window_emitter(Entity_editor_ctx& ctx): impl {new_unique<Impl>(*this, ctx)} {}
Window_emitter::~Window_emitter() {}
void Window_emitter::imgui_exec() { impl->imgui_exec(); }
void Window_emitter::update(const Delta_time dt) { impl->update(dt); }

