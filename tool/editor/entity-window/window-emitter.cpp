#include <imgui.h>
#include "window-emitter.hpp"
#include "entity-editor-ctx.hpp"
#include "entity-editor-util.hpp"
#include "util/hpw-util.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "scene/scene-entity-editor.hpp"
#include "game/entity/entity.hpp"
#include "game/core/entities.hpp"
#include "game/util/game-util.hpp"

namespace {
Uid g_wnd_uid {}; // чтобы ImGui не перепутал окна
}

struct Window_emitter::Impl {
  Entity_editor_ctx& m_ctx;
  Window_emitter& m_self;
  Scene_entity_editor& m_master;
  Uid m_uid {};
  Str m_title {}; // название окна для ImGui
  Phys m_spawn_phys {}; // параметры физики при спавне объектов
  Entity* m_last_entity {};

  std::size_t m_selected_name_id {};
  Str m_name_for_spawn {}; // выбранный entity

  inline explicit Impl(Window_emitter& self, Scene_entity_editor& master, Entity_editor_ctx& ctx)
  : m_ctx {ctx}
  , m_self {self}
  , m_master {master}
  , m_uid {get_uid()}
  {
    m_title = "Эмиттер " + n2s(m_uid);
    detailed_log("добавлено окно эмиттера (№" << m_uid << ")\n");
    m_spawn_phys.set_pos( get_screen_center() + Vec(0, 40) );
  }

  inline void imgui_exec() {
    ImGui::Begin(m_title.c_str(), {}, ImGuiWindowFlags_AlwaysAutoResize);
    Scope _({}, &ImGui::End);
    imgui_input();

    if (ImGui::BeginTabBar("emitter tab bar")) {
      Scope tab_bar_scope({}, &ImGui::EndTabBar);

      if (ImGui::BeginTabItem("основное")) {
        name_select();
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Phys спавна")) {
        phys_select(m_spawn_phys);
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Phys объекта")) {
        if (m_last_entity)
          phys_select(m_last_entity->phys);
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("флаги")) {
        if (m_last_entity)
          edit_flags(*m_last_entity);
        ImGui::EndTabItem();
      }
    } // BeginTabBar

    if (ImGui::Button("копировать"))
      copy_self();
    ImGui::SameLine();
    if (ImGui::Button("убрать объект"))
      kill_cur_entity();
    ImGui::SameLine();
    if (ImGui::Button("X"))
      exit();
  } // imgui_exec

  inline static Uid get_uid() { return ::g_wnd_uid++; }

  // выбирает доступный для спавна объект
  inline void name_select() {
    Strs names = m_ctx.entities_yml.root_tags();
    if (names.empty())
      names.push_back("entities.yml не содержит имён объектов");

    ImGui::Text("объект:");
    if (ImGui::BeginCombo("##объект", names.at(m_selected_name_id).c_str())) {
      for (uint i = 0; crauto name: names) {
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

  inline void exit() {
    kill_cur_entity();
    m_self.m_active = false;
  }

  // настраивает физику спавнера
  inline void phys_select(Phys& dst) {
    auto deg = dst.get_deg();
    auto speed = dst.get_speed();
    auto force = dst.get_force();
    auto accel = dst.get_accel();
    auto rot_frc = dst.get_rot_fc();
    auto rot_acc = dst.get_rot_ac();
    Vec pos = dst.get_pos();

    static_assert(std::is_same_v<decltype(deg), float>);
    static_assert(std::is_same_v<decltype(speed), float>);
    static_assert(std::is_same_v<decltype(force), float>);
    static_assert(std::is_same_v<decltype(accel), float>);
    static_assert(std::is_same_v<decltype(rot_frc), float>);
    static_assert(std::is_same_v<decltype(rot_acc), float>);
    static_assert(std::is_same_v<decltype(pos.x), float>);

    ImGui::Separator();
    float imgui_pos[2] {pos.x, pos.y};
    if (ImGui::SliderFloat2("позиция", imgui_pos, -50, 600)) {
      pos.x = imgui_pos[0];
      pos.y = imgui_pos[1];
    }
    ImGui::SliderFloat("направление", &deg, 0, 360);
    ImGui::SliderFloat("скорость", &speed, 0, 150_pps);
    ImGui::SliderFloat("ускорение", &accel, 0, 150_pps);
    ImGui::SliderFloat("торможение", &force, 0, 150_pps);
    ImGui::SliderFloat("ускор. вращ.", &rot_frc, 0, 720);
    ImGui::SliderFloat("тормо. вращ.", &rot_acc, 0, 150_pps);

    dst.set_deg(deg);
    dst.set_speed(speed);
    dst.set_force(force);
    dst.set_accel(accel);
    dst.set_rot_fc(rot_frc);
    dst.set_rot_ac(rot_acc);
    dst.set_pos(pos);
  } // phys_select

  inline void update(const Delta_time dt) {
    if (m_ctx.entities_is_clear)
      m_last_entity = {};
    update_spawner(dt);
  }

  inline void update_spawner(const Delta_time dt) {
    return_if(m_name_for_spawn.empty());
    return_if(m_last_entity && m_last_entity->status.live);

    m_last_entity = hpw::entity_mgr->make({}, m_name_for_spawn, m_spawn_phys.get_pos());
    return_if(!m_last_entity);
    m_last_entity->phys = m_spawn_phys;
  }

  inline void kill_cur_entity() {
    return_if(!m_last_entity || !m_last_entity->status.live);
    m_last_entity->kill();
  }

  inline void copy_self() {
    auto copy = new_unique<Window_emitter>(m_master, m_ctx);
    copy->impl->m_spawn_phys = m_spawn_phys;
    copy->impl->m_last_entity = {};
    copy->impl->m_selected_name_id = m_selected_name_id;
    copy->impl->m_name_for_spawn = m_name_for_spawn;
    m_master.move_emitter(std::move(copy));
  }
}; // Impl

Window_emitter::Window_emitter(Scene_entity_editor& master, Entity_editor_ctx& ctx)
: impl {new_unique<Impl>(*this, master, ctx)} {}
Window_emitter::~Window_emitter() {}
void Window_emitter::imgui_exec() { impl->imgui_exec(); }
void Window_emitter::update(const Delta_time dt) { impl->update(dt); }

