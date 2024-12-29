#include <imgui.h>
#include "wnd-ent-edit-opts.hpp"
#include "game/util/sync.hpp"
#include "game/core/core.hpp"
#include "game/core/graphic.hpp"
#include "game/core/debug.hpp"
#include "game/core/entities.hpp"
#include "entity-editor-ctx.hpp"
#include "util/hpw-util.hpp"
#include "host/host-util.hpp"

struct Wnd_ent_edit_opts::Impl {
  Entity_editor_ctx& m_ctx;

  inline explicit Impl(Entity_editor_ctx& ctx): m_ctx {ctx} {}

  inline void imgui_exec() {
    ImGui::Begin("Настройки", {}, ImGuiWindowFlags_AlwaysAutoResize);
    Scope _({}, &ImGui::End);

    if (ImGui::BeginTabBar("opts tab bar")) {
      Scope tab_bar_scope({}, &ImGui::EndTabBar);
      if (ImGui::BeginTabItem("редактор")) {
        process_editor_opts();
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("графика")) {
        process_graphic_opts();
        ImGui::EndTabItem();
      }
    } // BeginTabBar
  }

  inline void process_editor_opts() {
    // fast forward
    bool fast_forward = graphic::get_fast_forward();
    if (ImGui::Checkbox("ускорение", &fast_forward))
      graphic::set_fast_forward(fast_forward);

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

  inline void process_graphic_opts() {
    // VSync
    bool vsync = graphic::get_vsync();
    if (ImGui::Checkbox("V-sync", &vsync)) {
      graphic::set_vsync(vsync);
      if (!vsync)
        graphic::set_disable_frame_limit(true);
    }

    // haze
    bool enable_heat_distort = graphic::heat_distort_mode == Heat_distort_mode::disabled ? false : true;
    if (ImGui::Checkbox("выкл. искажение при лагах", &enable_heat_distort))
      graphic::heat_distort_mode = enable_heat_distort ? Heat_distort_mode::enabled : Heat_distort_mode::disabled;

    // frameskip
    ImGui::Checkbox("фреймскип при лагах", &graphic::auto_frame_skip);
    int frame_skip = graphic::frame_skip;
    ImGui::PushItemWidth(100);
    if (ImGui::InputInt("фреймскип", &frame_skip))
      graphic::frame_skip = std::max(0, frame_skip);
    ImGui::PopItemWidth();

    // wait frame
    if (ImGui::Checkbox("ждать конца кадра", &graphic::wait_frame))
      graphic::wait_frame_bak = graphic::wait_frame;

    // frame limit
    auto is_disable_frame_limit = graphic::get_disable_frame_limit();
    if (ImGui::Checkbox("выкл. лимит кадров", &is_disable_frame_limit))
      graphic::set_disable_frame_limit(is_disable_frame_limit);
    int fps = graphic::get_target_fps();
    ImGui::PushItemWidth(100);
    if (ImGui::InputInt("лимит кадров", &fps, 10)) {
      graphic::set_target_fps(std::max(1, fps));
      graphic::set_disable_frame_limit(false);
    }
    ImGui::PopItemWidth();

    // tickrate
    int ups = hpw::target_ups;
    ImGui::PushItemWidth(100);
    if (ImGui::InputInt("тикрейт", &ups, 10))
      set_target_ups(std::max(1, ups));
    ImGui::PopItemWidth();

    // blink particles
    ImGui::Checkbox("мигать частицами", &graphic::blink_particles);
  } // process_graphic_opts
}; // Impl

Wnd_ent_edit_opts::Wnd_ent_edit_opts(Entity_editor_ctx& ctx): impl {new_unique<Impl>(ctx)} {}
Wnd_ent_edit_opts::~Wnd_ent_edit_opts() {}
void Wnd_ent_edit_opts::imgui_exec() { impl->imgui_exec(); }
