#include <cassert>
#include <algorithm>
#include <imgui.h>
#include "window-util.hpp"
#include "window-global.hpp"
#include "game/util/sync.hpp"
#include "game/entity/entity.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/image/image.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/animation/frame.hpp"
#include "util/hpw-util.hpp"

void draw_frames(Pool_ptr(Entity) entity, Image& dst, const Vec pos) {
  assert(dst);
  assert(entity);
  auto anim = editor::entity->anim_ctx.get_anim();
  return_if (!anim);
  crauto frames = anim->get_frames();
  Vec pos2 {pos};

  for (crauto frame: frames) {
    int max_frame_y {0};
    continue_if ( !frame);
    auto directions = frame->get_directions();
    continue_if (directions.empty());
    for (crauto direct: directions) {
      crauto sprite = direct.sprite;
      continue_if(sprite.expired());
      switch (editor::draw_frames_mode) {
        default:
        case Draw_frames_mode::sprite:
          insert(dst, *sprite.lock(), pos2);
          break;
        case Draw_frames_mode::mask:
          insert(dst, sprite.lock()->mask(), pos2);
          break;
        case Draw_frames_mode::image:
          insert(dst, sprite.lock()->image(), pos2);
          break;
      }
      if (editor::use_draw_cross && (graphic::frame_count & 0b100))
        draw_cross<&blend_diff>(dst, pos2 - direct.offset, Pal8::white, 3);
      pos2.x += sprite.lock()->X() + 2;
      max_frame_y = std::max(sprite.lock()->Y(), max_frame_y);
    } // for directs
    pos2.y += max_frame_y + 2;
    pos2.x = pos.x;
  } // for frames
} // draw_frames

Anim* get_anim() {
  auto anim = editor::entity->anim_ctx.get_anim();
  if (!anim)
    return {};
  return ccast<Anim*>(anim);
}

void Flag_editor::emplace(get_flag_pf&& get_f, set_flag_pf&& set_f, CN<Str> name) {
  assert(get_f);
  assert(set_f);
  m_flags.emplace_back( Flag {
    .get_f {std::move(get_f)},
    .set_f {std::move(set_f)},
    .name {name}
  } );
}

void Flag_editor::accept() {
  return_if(m_accepted);

  // перенести флаги к себе
  for (rauto flag: m_flags)
    flag.value = flag.get_f();

  // построить таблицу в imgui
  Scope end_table_scope({}, &ImGui::EndTable);
  ImGui::Text("flags:");
  if (ImGui::BeginTable("flags", 3,
    ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings |
    ImGuiTableFlags_Borders))
  {
    for (rauto flag: m_flags) {
      ImGui::TableNextColumn();
      ImGui::Selectable(flag.name.c_str(), &flag.value);    
    }
  }

  // перенести флаги обратно
  for (crauto flag: m_flags)
    flag.set_f(flag.value);
  
  m_accepted = true;
} // accept

Flag_editor::~Flag_editor() { accept(); }
