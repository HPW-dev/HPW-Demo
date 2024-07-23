#include <imgui.h>
#include "wnd-ent-edit-opts.hpp"
#include "game/util/sync.hpp"
#include "game/core/debug.hpp"
#include "game/core/entities.hpp"
#include "entity-editor-ctx.hpp"
#include "util/hpw-util.hpp"

struct Wnd_ent_edit_opts::Impl {
  Entity_editor_ctx& m_ctx;

  inline explicit Impl(Entity_editor_ctx& ctx): m_ctx {ctx} {}

  inline void imgui_exec() {
    ImGui::Begin("Настройки", {}, ImGuiWindowFlags_AlwaysAutoResize);
    Scope _({}, &ImGui::End);

    // VSync
    bool vsync = graphic::get_vsync();
    if (ImGui::Checkbox("V-sync", &vsync)) {
      graphic::set_vsync(vsync);
      if (!vsync)
        graphic::set_disable_frame_limit(true);
    }
    // entity editor pause
    ImGui::Checkbox("пауза", &m_ctx.pause);
    ImGui::Checkbox("хитбоксы", &graphic::draw_hitboxes);
    ImGui::Checkbox("сетка системы коллизий", &graphic::show_grids);
    if(ImGui::Button("убрать все объекты")) {
      hpw::entity_mgr->clear_entities();
      m_ctx.entities_is_clear = true;
    }
    // настройки фона
    ImGui::Checkbox("красный оттенок фона", &m_ctx.red_bg);
    ImGui::Text("яркость фона");
    ImGui::SliderFloat("##яркость фона", &m_ctx.bg_color, 0, 1);
  }
}; // Impl

Wnd_ent_edit_opts::Wnd_ent_edit_opts(Entity_editor_ctx& ctx): impl {new_unique<Impl>(ctx)} {}
Wnd_ent_edit_opts::~Wnd_ent_edit_opts() {}
void Wnd_ent_edit_opts::imgui_exec() { impl->imgui_exec(); }
