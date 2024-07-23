#include <imgui.h>
#include "window-emitter.hpp"
#include "entity-editor-ctx.hpp"
#include "util/hpw-util.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"

namespace {
Uid g_wnd_uid {}; // чтобы ImGui не перепутал окна
}

struct Window_emitter::Impl {
  Entity_editor_ctx& m_ctx;
  Window_emitter& m_master;
  Uid m_uid {};
  Str m_title {}; // название окна для ImGui

  std::size_t m_selected_name_id {};
  Str m_name_for_spawn {}; // выбранный entity

  inline explicit Impl(Window_emitter& master, Entity_editor_ctx& ctx)
  : m_ctx {ctx}
  , m_master {master}
  , m_uid {get_uid()}
  {
    m_title = "Эмиттер " + n2s(m_uid);
    detailed_log("добавлено окно эмиттера (№" << m_uid << ")\n");
  }

  inline void imgui_exec() {
    ImGui::Begin(m_title.c_str(), {}, ImGuiWindowFlags_AlwaysAutoResize);
    Scope _({}, &ImGui::End);

    name_select();

    if (ImGui::Button("Закрыть")) {
      m_master.m_active = false;
      return;
    }
  }

  inline static Uid get_uid() { return ::g_wnd_uid++; }

  // выбирает доступный для спавна объект
  inline void name_select() {
    Strs names = m_ctx.entities_yml.root_tags();
    if (names.empty())
      names.push_back("entities.yml is empty");

    ImGui::Text("entity:");
    if (ImGui::BeginCombo("##entity", names.at(m_selected_name_id).c_str())) {
      for (uint i; cnauto name: names) {
        const bool is_selected = (i == m_selected_name_id);
        if (ImGui::Selectable(name.c_str(), is_selected))
          m_selected_name_id = i;
        if (is_selected)
          ImGui::SetItemDefaultFocus();
        ++i;
      }
      ImGui::EndCombo();
    } // if BeginCombo

    m_name_for_spawn = names.at(m_selected_name_id);
  }
}; // Impl

Window_emitter::Window_emitter(Entity_editor_ctx& ctx): impl {new_unique<Impl>(*this, ctx)} {}
Window_emitter::~Window_emitter() {}
void Window_emitter::imgui_exec() { impl->imgui_exec(); }

