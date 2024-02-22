#include <imgui.h>
#include <vector>
#include <algorithm>
#include "emit.hpp"
#include "window-global.hpp"
#include "window-util.hpp"
#include "scene/input.hpp"
#include "util/hpw-util.hpp"
#include "util/log.hpp"
#include "game/util/game-util.hpp"
#include "game/core/canvas.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "game/entity/util/phys.hpp"
#include "game/scene/scene-manager.hpp"
#include "util/file/yaml.hpp"
#include "util/str-util.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/animation/animation-manager.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/effect/heat-distort.hpp"
#include "graphic/effect/light.hpp"

Emit_wnd::Emit_wnd() {
  reset();
}

void Emit_wnd::imgui_exec() {
  ImGui::Begin("Spawner", {}, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::BeginTabBar("entity_config#tab_bar");
  Scope imgui_scope({}, &ImGui::End);
  Scope imgui_tab_scope({}, &ImGui::EndTabBar);

  if (ImGui::BeginTabItem("main")) {
    Scope tab_scope({}, &ImGui::EndTabItem);
    draw_spawner_types();
    draw_anim_list();
    ImGui::SameLine();
    if (ImGui::Button("+"))
      make_new_anim();
    ImGui::SameLine();
    if (ImGui::Button("-"))
      delete_anim_by_name(editor::anim_name);
  }

  if (ImGui::BeginTabItem("flags")) {
    Scope tab_scope({}, &ImGui::EndTabItem);
    draw_entity_flags();
  }

  if (ImGui::BeginTabItem("light")) {
    Scope tab_scope({}, &ImGui::EndTabItem);
    light_edit();
  }

  if (ImGui::BeginTabItem("distort")) {
    Scope tab_scope({}, &ImGui::EndTabItem);
    heat_distort_edit();
  }
} // imgui_exec

void Emit_wnd::reset() {
  editor::entity = hpw::entity_mgr->allocate<Collidable>();
  editor::entity->phys.set_pos(center_point(*graphic::canvas));
  select_anim_by_name(get_avaliable_first_anim());
}

Strs spawner_type_names() {
  Strs list;
  for (cnauto item: spawner_type_table)
    list.emplace_back(item.second);
  return list;
}

void Emit_wnd::draw_spawner_types() {
  int item {scast<int>(type)};
  auto list = strs_to_null_terminated(spawner_type_names());
  ImGui::Text("type");
  ImGui::Combo("##type", &item, list.data());
  type = scast<spawner_t>(item);
  //TODO del:
  if (type != spawner_t::statical) {
    hpw_log("[!] need impl for other spawners!\n");
    type = spawner_t::statical;
  }
}

void Emit_wnd::draw_anim_list() {
  auto list = hpw::anim_mgr->names();
  std::sort(list.begin(), list.end());
  auto list_for_combo = strs_to_null_terminated(list);

  int list_idx = 0;
  for (cnauto name: list) {
    if (name == editor::anim_name)
      break;
    ++list_idx;
  }

  ImGui::Text("animation");
  if (ImGui::Combo("##animation", &list_idx, list_for_combo.data()) )
    select_anim_by_name(list.at(list_idx));
}

void Emit_wnd::delete_anim_by_name(CN<Str> name) {
  return_if (name.empty());
  detailed_log("delete anim \"" << name << "\n");
  hpw::anim_mgr->remove_anim(name);
  select_anim_by_name(get_avaliable_first_anim());
}

void Emit_wnd::select_anim_by_name(CN<Str> name) {
  return_if(name.empty());
  editor::anim_name = name;
  auto anim = hpw::anim_mgr->find_anim(editor::anim_name).get();
  assert(anim);
  editor::entity->anim_ctx.set_anim(anim);
  detailed_log("select anim \"" << editor::anim_name << "\n");
}

void Emit_wnd::make_new_anim() {
  hpw::scene_mgr->add(
    new_shared<Input_scene>(
      "Enter animation name",
      [this](CN<Str> name) {
        auto low_name = str_tolower(name);
        auto anim {new_shared<Anim>()};
        try {
          hpw::anim_mgr->add_anim(low_name, anim);
        } catch (CN<hpw::Error> ex) {
          hpw_log(ex.get_msg() << '\n');
          return;
        }
        select_anim_by_name(low_name);
      }
    ) // Input_scene c-tor
  ); // shared c-tor
} // make_new_anim

void Emit_wnd::draw_entity_flags() {
  return_if (!editor::entity);
  auto& status = editor::entity->status;
  Flag_editor flag_editor;
  
  #define flag_editor_emplace(flag, title) \
    flag_editor.emplace ( \
      [status]->bool { return status.flag; }, \
      [&status](bool val) { status.flag = val; }, \
      title \
    );
  flag_editor_emplace(live, "live")
  flag_editor_emplace(end_anim, "end anim")
  flag_editor_emplace(kill_by_end_anim, "dead by end anim")
  flag_editor_emplace(end_frame, "end frame")
  flag_editor_emplace(layer_up, "layer up")
  flag_editor_emplace(rnd_frame, "random frame")
  flag_editor_emplace(rnd_deg, "random degree")
  flag_editor_emplace(rnd_deg_evr_frame, "rnd. deg. evr. frm.")
  flag_editor_emplace(stop_anim, "stop anim")
  flag_editor_emplace(return_back, "return back")
  flag_editor_emplace(goto_prev_frame, "to prev frame")
  #undef flag_editor_emplace
} // draw_entity_flags

void Emit_wnd::heat_distort_edit() {
  auto enable = scast<bool>(editor::entity->heat_distort);
  if (ImGui::Checkbox("enable heat distort", &enable)) {
    if (enable) {
      editor::entity->heat_distort = new_shared<Heat_distort>();
      auto& heat_distort = *editor::entity->heat_distort;
      heat_distort.block_count = 6;
      heat_distort.block_size = 32;
      heat_distort.radius = 60;
      heat_distort.set_duration(10);
      heat_distort.power = 5;
    } else {
      editor::entity->heat_distort = {};
    }
  } // if checkbox enable
  return_if (!enable);

  Heat_distort& heat_distort = *editor::entity->heat_distort;
  int block_count = heat_distort.block_count;
  int block_size = heat_distort.block_size;
  int radius = heat_distort.radius;
  if (ImGui::SliderInt("radius", &radius, 1, 600))
    heat_distort.radius = radius;
  if (ImGui::SliderInt("block size", &block_size, 1, 500))
    heat_distort.block_size = block_size;
  if (ImGui::SliderInt("block count", &block_count, 1, 150))
    heat_distort.block_count = block_count;
  ImGui::SliderFloat("power", &heat_distort.power, 0, 30);
  float duration = heat_distort.get_max_duration();
  if (ImGui::SliderFloat("duration", &duration, 0, 10))
    heat_distort.set_duration(duration);
  // показать сколько прошло времени эффекта
  ImGui::ProgressBar(heat_distort.get_cur_duration() /
    heat_distort.get_max_duration());

  heat_distort_flags_edit(heat_distort);

  if (ImGui::Button("restart"))
    heat_distort.restart();
  ImGui::SameLine();
  if (ImGui::Button("save to clipboard"))
    save_to_clipboard(heat_distort);
} // heat_distort_edit

void Emit_wnd::heat_distort_flags_edit(Heat_distort& heat_distort) {
  auto& flags = heat_distort.flags;
  Flag_editor flag_editor;

  #define flag_editor_emplace(flag, title) \
    flag_editor.emplace ( \
      [flags]->bool { return flags.flag; }, \
      [&flags](bool val) { flags.flag = val; }, \
      title \
    );
  flag_editor_emplace(random_block_count,     "rnd. block cnt")
  flag_editor_emplace(random_radius,          "rnd. radius")
  flag_editor_emplace(random_block_size,      "rnd. block sz.")
  flag_editor_emplace(random_power,           "rnd. power")
  flag_editor_emplace(infinity_duration,      "infini. dura.")
  flag_editor_emplace(decrease_radius,        "dec. radius")
  flag_editor_emplace(decrease_power,         "dec. power")
  flag_editor_emplace(decrease_block_size,    "dec. block sz.")
  flag_editor_emplace(invert_decrease_radius, "inv. dec. radius")
  flag_editor_emplace(repeat,                 "repeat")
  #undef flag_editor_emplace
} // heat_distort_flags_edit

void Emit_wnd::light_edit() {
  auto enable = scast<bool>(editor::entity->light);
  if (ImGui::Checkbox("enable light", &enable)) {
    if (enable) {
      editor::entity->light = new_shared<Light>();
      Light& light = *editor::entity->light;
      light.set_duration(10);
      light.radius = 64;
    } else {
      editor::entity->light = {};
    }
  } // if checkbox enable
  return_if (!enable);

  Light& light = *editor::entity->light;

  float duration = light.get_max_duration();
  if (ImGui::SliderFloat("duration", &duration, 0, 10))
    light.set_duration(duration);
  
  int radius = light.radius;
  if (ImGui::SliderInt("radius", &radius, 0, 600))
    light.radius = radius;
} // light_edit

void Emit_wnd::save_to_clipboard(CN<Heat_distort> src) {
  std::stringstream txt;

  txt << "heat_distort:\n";
  txt << "  max_duration: " << src.get_max_duration() << "\n";
  txt << "  radius: " << src.radius << "\n";
  txt << "  block_size: " << src.block_size << "\n";
  txt << "  block_count: " << src.block_count << "\n";
  txt << "  power: " << src.power << "\n";

  txt << std::boolalpha;
  txt << "  flags:\n";
  txt << "    random_block_count: " << src.flags.random_block_count << "\n";
  txt << "    random_radius: " << src.flags.random_radius << "\n";
  txt << "    random_block_size: " << src.flags.random_block_size << "\n";
  txt << "    random_power: " << src.flags.random_power << "\n";
  txt << "    infinity_duration: " << src.flags.infinity_duration << "\n";
  txt << "    decrease_radius: " << src.flags.decrease_radius << "\n";
  txt << "    invert_decrease_radius: " << src.flags.invert_decrease_radius << "\n";
  txt << "    decrease_power: " << src.flags.decrease_power << "\n";
  txt << "    decrease_block_size: " << src.flags.decrease_block_size << "\n";
  txt << "    repeat: " << src.flags.repeat << "\n";

  ImGui::SetClipboardText(txt.str().c_str());
} // save_to_clipboard

Str Emit_wnd::get_avaliable_first_anim() const {
  auto list = hpw::anim_mgr->names();
  if (list.empty())
    return {};
  std::sort(list.begin(), list.end());
  return list.at(0);
}
