#include <imgui.h>
#include <algorithm>
#include "frame-edit.hpp"
#include "window-global.hpp"
#include "util/hpw-util.hpp"
#include "util/math/polygon.hpp"
#include "util/str-util.hpp"
#include "util/log.hpp"
#include "game/core/debug.hpp"
#include "game/core/scenes.hpp"
#include "game/core/sprites.hpp"
#include "game/core/entities.hpp"
#include "game/scene/scene-manager.hpp"
#include "game/entity/entity.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/hitbox.hpp"
#include "game/util/store.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/animation/frame.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/util/graphic-util.hpp"
#include "scene/sprite-select.hpp"
#include "window-util.hpp"

void Frame_wnd::imgui_exec() {
  ImGui::Begin("Frame", {}, ImGuiWindowFlags_AlwaysAutoResize);
  Scope _({}, &ImGui::End);

  // в редакторе константный фрейм можно менять
  auto frame_p = editor::entity->anim_ctx.get_cur_frame();
  return_if ( !frame_p);
  auto& frame = *ccast<Frame*>(frame_p);

  ImGui::BeginTabBar("frame_config#tab_bar");
  Scope imgui_tab_scope({}, &ImGui::EndTabBar);

  // вкладка настроек спрайтов и длительности анимации
  if (ImGui::BeginTabItem("animation")) {
    Scope tab_scope({}, &ImGui::EndTabItem);

    draw_info(frame);
    draw_direction_count_edit(frame);
    draw_path_edit(frame);
    draw_downscale_tune_edit(frame);
    draw_offset_edit(frame);
    ImGui::Separator();
    draw_duration_edit(frame);
  }

  // вкладка настроек хитбокса
  if (ImGui::BeginTabItem("hitbox")) {
    Scope tab_scope({}, &ImGui::EndTabItem);
    edit_hitbox(frame);
  }
} // imgui_exec

void Frame_wnd::draw_info(CN<Frame> frame) {
  Str name = "name: " + frame.get_name();
  Str duration = "duration: " + n2s(frame.duration, 3);
  ImGui::Text(name.c_str());
  ImGui::Text(duration.c_str());

// отобразить разрешение спрайта
  Vec res;
  // эти ифы для проверки что есть фрейм у анимации
  if (auto direct = frame.get_direct(editor::entity->phys.get_deg()); direct) {
    if (auto sprite = direct->sprite; !sprite.expired()) {
      cnauto image = sprite.lock()->image();
      res = Vec(image.X, image.Y);
    }
  }
  Str resolution = "resolution: " + n2s(res.x, 0) + "x" + n2s(res.y, 0);
  ImGui::Text(resolution.c_str());
} // draw_info

void Frame_wnd::draw_direction_count_edit(Frame& frame) {
  auto directions = frame.get_directions();

  // настройка количества поворотов в анимации
  int max_directions = directions.size();
  if (ImGui::InputInt("directions", &max_directions,
    1, 1, ImGuiInputTextFlags_EnterReturnsTrue)
  ) {
    max_directions = std::clamp(max_directions, 0, 128);
    detailed_log("set new direction count for \"" << frame.get_name() << "\" - "
      << max_directions << "\n");
    frame.source_ctx.max_directions = max_directions;
    frame.init_directions( std::move(frame.source_ctx) );
  }
  // применить настройку ко всем кадрам анимации
  ImGui::SameLine();
  ImGui::PushID(1);
  if (ImGui::Button("all")) {
    auto anim = editor::entity->anim_ctx.get_anim();
    if (anim) {
      auto all_frames = anim->get_frames();
      for (cnauto const_frame: all_frames) {
        auto cp_frame = const_frame.get();
        cont_if (!cp_frame);
        auto& ref_frame = *ccast<Frame*>(cp_frame);
        ref_frame.source_ctx.max_directions = max_directions;
        ref_frame.init_directions( std::move(ref_frame.source_ctx) );
      }
    }
  }
  ImGui::PopID();
} // draw_direction_count_edit

void Frame_wnd::draw_path_edit(Frame& frame) {
  // добавить спрайт через поисковик
  if (ImGui::Button("+")) {
    hpw::scene_mgr->add(
      new_shared<Sprite_select>(
        [this, &frame](Shared<Sprite> sprite, CN<Str> path) {
          detailed_log("new sprite selected for frame \"" << frame.get_name() << "\"\n");
          auto new_ctx = frame.source_ctx;
          new_ctx.direct_0.sprite = sprite;
          // чтобы показать хоть какой-то спрайт
          new_ctx.max_directions = std::max<std::size_t>(new_ctx.max_directions, 1);
          frame.init_directions( std::move(new_ctx) );
        }
      )
    );
  } // if bootom +

  ImGui::SameLine();
  if (ImGui::Button("-"))
    frame.clear_directions();

  // поле ввода пути к спрайту
  std::array<char, 1024> path_buffer {};
  if (auto frame_sprite = frame.source_ctx.direct_0.sprite; !frame_sprite.expired())
    to_null_ended(path_buffer, frame_sprite.lock()->get_path());
  ImGui::SameLine();
  if ( ImGui::InputText("##file", path_buffer.data(), path_buffer.size(),
    ImGuiInputTextFlags_EnterReturnsTrue)
  ) {
    auto new_path = from_null_ended<Str>(path_buffer);
    auto sprite = hpw::store_sprite->find(new_path);
    if (sprite) {
      frame.source_ctx.direct_0.sprite = sprite;
      frame.reinit_directions_by_source();
    } else {
      hpw_log("bad sprite path name (\"" << new_path << "\")\n");
    }
  } // if imgui::input (file path)
} // draw_path_edit

void Frame_wnd::draw_offset_edit(Frame& frame) {
  auto &cur_offset = frame.source_ctx.direct_0.offset;
  auto &source_rotate_offset = frame.source_ctx.rotate_offset;
  bool reinit = false;
  ImGui::PushItemWidth(120);

  // редактирование смещения для генерации поворотов
  if (ImGui::InputFloat("##gen. offset X", &source_rotate_offset.x, 0.1, 0.25) ) {
    source_rotate_offset.x = std::clamp<real>(source_rotate_offset.x, -5, 5);
    reinit = true;
  }
  ImGui::SameLine();
  if (ImGui::InputFloat("gen. offset", &source_rotate_offset.y, 0.1, 0.25) ) {
    source_rotate_offset.y = std::clamp<real>(source_rotate_offset.y, -5, 5);
    reinit = true;
  }

  // редактирование смещения для отрисовки
  if (ImGui::InputFloat("##offset X", &cur_offset.x, 1, 4) )
    reinit = true;
  ImGui::SameLine();
  if (ImGui::InputFloat("draw offset", &cur_offset.y, 1, 4) )
    reinit = true;

  ImGui::PopItemWidth();
  if (reinit)
    frame.reinit_directions_by_source();
} // draw_offset_edit

void Frame_wnd::draw_downscale_tune_edit(Frame& frame) {
  ImGui::PushItemWidth(135);

  // список режимов семплирования исходника
  int cgp_sel = scast<int>(frame.source_ctx.cgp);
  sconst Str cgp_strings = strs_to_null_terminated({
    "cross",
    "box",
  });
  if (ImGui::Combo("CGP", &cgp_sel, cgp_strings.c_str()) ) {
    frame.source_ctx.cgp = decltype(frame.source_ctx.cgp) (cgp_sel);
    frame.reinit_directions_by_source();
  }

  // список режимов определения цвета при уменьшении
  int ccf_sel = scast<int>(frame.source_ctx.ccf);
  sconst Str ccf_strings = strs_to_null_terminated({
    "most common",
    "max",
    "min",
    "average",
  });
  ImGui::SameLine();
  if (ImGui::Combo("CCF", &ccf_sel, ccf_strings.c_str()) ) {
    frame.source_ctx.ccf = decltype(frame.source_ctx.ccf) (ccf_sel);
    frame.reinit_directions_by_source();
  }

  ImGui::PopItemWidth();
} // draw_downscale_tune_edit

void Frame_wnd::draw_duration_edit(Frame& frame) {
  if (ImGui::InputFloat("duration", &frame.duration, 0.001, 0.1)) {
    frame.duration = std::max<real>(0, frame.duration);
  }
  ImGui::SameLine();
  ImGui::PushID(2);
  if (ImGui::Button("all")) {
    auto new_duration = frame.duration;
    auto anim = editor::entity->anim_ctx.get_anim();
    if (anim) {
      auto all_frames = anim->get_frames();
      for (cnauto const_frame: all_frames) {
        cont_if (!const_frame);
        auto& ref_frame = *ccast<Frame*>(const_frame.get());
        ref_frame.duration = new_duration;
      }
    }
  } // if button "all"
  ImGui::PopID();
} // draw_duration_edit

Pool_ptr(Hitbox) Frame_wnd::get_hitbox_source() const {
  auto anim = get_anim();

  // создать хитбокс, если его нет
  auto hitbox_source = anim->get_hitbox_source();
  if ( !hitbox_source) {
    anim->update_hitbox(hpw::entity_mgr->get_hitbox_pool().new_object<Hitbox>(), 1);
    hitbox_source = anim->get_hitbox_source();
    graphic::draw_hitboxes = true; // заодно включить показ хитбоксов на экране
  }
  return hitbox_source;
} // get_hitbox_source

bool Frame_wnd::edit_polygon(Pool_ptr(Hitbox) hitbox) const {
  bool ret {false};

  ImGui::Text("polygon actions: ");
  // добавить полигон
  ImGui::SameLine();
  if (ImGui::Button("+##poly")) {
    hitbox->polygons.emplace_back(Polygon{
      .points=Vector<Vec>(3) // сразу накинуть 3 точки
    } );
    ret = true;
  }
  // убрать полигон
  ImGui::SameLine();
  if (ImGui::Button("-##poly")) {
    if (!hitbox->polygons.empty()) {
      hitbox->polygons.pop_back();
      ret = true;
    }
  }
  // копировать последний полигон
  ImGui::SameLine();
  if (ImGui::Button("=##poly")) {
    if (!hitbox->polygons.empty()) {
      hitbox->polygons.push_back(hitbox->polygons.back());
      ret = true;
    }
  }
  // сделать симметрично по горизонтали
  ImGui::SameLine();
  if (ImGui::Button("|##poly")) {
    if (!hitbox->polygons.empty()) {
      hitbox->make_h_symm();
      ret = true;
    }
  }

  return ret;
} // edit_polygon

bool Frame_wnd::edit_polygon_offset(Polygon& poly) const {
  bool ret {false};

  // оффсет полигона
  static_assert(typeid(decltype(poly.offset.x)) == typeid(float));
  ImGui::Text("offset x/y:");
  ImGui::PushItemWidth(120);
  if (ImGui::InputFloat("##offset-x", &poly.offset.x, 1, 2))
    ret = true;
  ImGui::SameLine();
  if (ImGui::InputFloat("##offset-y", &poly.offset.y, 1, 2))
    ret = true;
  ImGui::PopItemWidth();

  return ret;
} // edit_polygon_offset

bool Frame_wnd::edit_inserter_polygon_points(Polygon& poly) const {
  bool ret {false};

  // добавить точку
  if (ImGui::Button("+##point")) {
    poly.points.emplace_back(Vec{});
    ret = true;
  }
  // убрать точку
  ImGui::SameLine();
  if (ImGui::Button("-##point")) {
    if (!poly.points.empty()) {
      poly.points.pop_back();
      ret = true;
    }
  }
  // скопировать последнюю точку
  ImGui::SameLine();
  if (ImGui::Button("=##point")) {
    if (!poly.points.empty()) {
      poly.points.push_back(poly.points.back());
      ret = true;
    }
  }

  ImGui::SameLine();
  ImGui::Text("point");

  return ret;
} // edit_inserter_polygon_points

bool Frame_wnd::edit_polygon_detailed(Pool_ptr(Hitbox) hitbox) const {
  bool ret {false};

  // скролящаяся область
  ImVec2 scrolling_child_size(400, 300);
  ImGui::BeginChild("edit hitbox", scrolling_child_size, true);
  Scope end_child({}, &ImGui::EndChild);

  // все полигоны хитбокса
  for (auto poly_id = 0; nauto poly: hitbox->polygons) {
    auto poly_name = "Polygon id: " + n2s(poly_id);
    ImGui::SetNextItemOpen(true, ImGuiCond_Once); // открыть ноду сразу
    if (ImGui::TreeNode(poly_name.c_str())) {
      ret |= edit_inserter_polygon_points(poly);
      ret |= edit_polygon_offset(poly);

      // пройтись по точкам
      for (uint point_id = 0; nauto point: poly.points) {
        auto point_name = "point id: " + n2s(point_id);
        ImGui::SetNextItemOpen(true, ImGuiCond_Once); // открыть ноду сразу
        if (ImGui::TreeNode(point_name.c_str())) {
          // настройка кордов точки
          ImGui::PushItemWidth(120);
          if (ImGui::InputFloat("##pos x", &point.x, 0.5, 1))
            ret = true;
          ImGui::SameLine();
          if (ImGui::InputFloat("pos", &point.y, 0.5, 1))
            ret = true;
          ImGui::PopItemWidth();
          ImGui::TreePop(); // point
        }
        ++point_id;
      }
      ImGui::TreePop(); // poly
      ++poly_id;
    } // for poly
  } // edit_hitbox

  return ret;
} // edit_polygon_detaile

void Frame_wnd::update_hitbox(Pool_ptr(Hitbox) dst) {
  auto anim = get_anim();

  /* вычислить максимум направлений в кадрах анимации и
  сделать столько же углов разворота для хитбокса */
  uint Hitbox_directions_count = 1;
  for (cnauto frame: anim->get_frames()) {
    cont_if (!frame);
    auto directions_count = frame->get_directions().size();
      Hitbox_directions_count = std::max<uint>(Hitbox_directions_count, directions_count);
  }

  anim->update_hitbox( hpw::entity_mgr->get_hitbox_pool().new_object<Hitbox>(*dst),
    Hitbox_directions_count );
} // update_hitbox

void Frame_wnd::edit_hitbox(Frame& frame) {
  auto hitbox = get_hitbox_source();
  if ( !hitbox) {
    hpw_log("unable to set hitbox for entity\n");
    return;
  }

  bool updated = false;
  updated |= edit_polygon(hitbox);
  updated |= edit_polygon_detailed(hitbox);

  if (updated) 
    update_hitbox(hitbox);
} // edit_hitbox
