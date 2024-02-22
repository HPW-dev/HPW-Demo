#include <imgui.h>
#include "window-anim.hpp"
#include "window-global.hpp"
#include "window-util.hpp"
#include "util/hpw-util.hpp"
#include "util/math/mat.hpp"
#include "game/entity/entity.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/animation/frame.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/str-util.hpp"

void Anim_wnd::imgui_exec() {
  using namespace ImGui;
  Begin("Animation", {}, ImGuiWindowFlags_AlwaysAutoResize);
  Scope _({}, &ImGui::End);
  edit_start_deg();
  draw_speed_scale();
  Separator();
  Text("frame order:");
  if (Button("+"))
    make_new_frame();
  SameLine();
  if (Button("-"))
    delete_cur_frame();
  SameLine();
  if (Button("swap back"))
    move_cur_frame_up();
  SameLine();
  if (Button("swap next"))
    move_cur_frame_down();
  SameLine();
  if (Button("copy"))
    copy_cur_frame_to_next();
  draw_frame_dur();
  draw_frame_num();
} // imgui_exec

void Anim_wnd::update(double dt) {
  editor::entity->status.stop_anim = editor::is_pause;
}

void Anim_wnd::edit_start_deg() {
  float degree = editor::entity->phys.get_deg();
  ImGui::SliderFloat("degree", &degree, 0, 360, "%0.2f");
  editor::entity->phys.set_deg(degree);
}

void Anim_wnd::make_new_frame() {
  auto anim = get_anim();
  return_if( !anim);
  auto frame_num = editor::entity->anim_ctx.get_cur_frame_idx();
  auto frame = new_shared<Frame>();
  anim->insert(frame_num+1, frame);
  editor::entity->anim_ctx.set_cur_frame(frame_num+1);
}

void Anim_wnd::delete_cur_frame() {
  auto anim = get_anim();
  return_if(!anim);

  auto frame_num = editor::entity->anim_ctx.get_cur_frame_idx();
  anim->remove_frame(frame_num);
  editor::entity->anim_ctx.set_cur_frame(frame_num - 1);
}

void Anim_wnd::move_cur_frame_up() {
  auto anim = get_anim();
  return_if(!anim);
  auto frame_num = editor::entity->anim_ctx.get_cur_frame_idx();
  anim->swap_frame(frame_num, frame_num + 1);
}

void Anim_wnd::move_cur_frame_down() {
  auto anim = get_anim();
  return_if(!anim);

  auto frame_num = editor::entity->anim_ctx.get_cur_frame_idx();
  anim->swap_frame(frame_num, frame_num - 1);
}

bool Anim_wnd::draw_frame_num() {
  auto anim = get_anim();
  if (!anim)
    return false;

  auto frames = anim->get_frames();
  Str txt = "frames: " + n2s(frames.size());
  ImGui::Text(txt.c_str());
  if (frames.empty())
    return false;
  int frame_num = editor::entity->anim_ctx.get_cur_frame_idx();
  if (ImGui::SliderInt("cur frame", &frame_num, 0, frames.size()-1))
    editor::entity->anim_ctx.set_cur_frame(frame_num);
  return true;
}

void Anim_wnd::draw_speed_scale() {
  float speed_scale = editor::entity->anim_ctx.get_speed_scale();
  if (ImGui::InputFloat("speed scale", &speed_scale, 0.005f, 3.0f, "%0.3f"))
    editor::entity->anim_ctx.set_speed_scale(speed_scale);
}

void Anim_wnd::draw_frame_dur() {
  auto anim = editor::entity->anim_ctx.get_anim();
  return_if (!anim);

  // посчитать длительность каждого кадра и умножить на speed_scale
  real total_duration = 0;
  for (auto frames = anim->get_frames(); cnauto frame: frames)
    if (frame)
      total_duration += frame->duration;
  total_duration *= safe_div(1.0, editor::entity->anim_ctx.get_speed_scale());
  Str txt = "total duration: " + n2s(total_duration, 4);
  ImGui::Text(txt.c_str());
}

void Anim_wnd::copy_cur_frame_to_next() {
  auto anim = get_anim();
  return_if(!anim);

  auto frame_num = editor::entity->anim_ctx.get_cur_frame_idx();
  auto copy = new_shared<Frame>(*anim->get_frame(frame_num));
  anim->add_frame(copy);
  editor::entity->anim_ctx.set_cur_frame(frame_num + 1);
}
