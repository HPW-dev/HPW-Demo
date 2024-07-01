#ifndef __clang__
#include <omp.h>
#endif

#include <cmath>
#include "window-util.hpp"
#include "root.hpp"
#include "menu.hpp"
#include "opts.hpp"
#include "emit.hpp"
#include "window-anim.hpp"
#include "frame-edit.hpp"
#include "window-global.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/util/sync.hpp"
#include "game/core/scenes.hpp"
#include "game/core/anims.hpp"
#include "game/core/entities.hpp"
#include "game/core/canvas.hpp"
#include "game/core/debug.hpp"
#include "game/core/fonts.hpp"
#include "game/core/common.hpp"
#include "game/util/game-util.hpp"
#include "game/util/camera.hpp"

Root_wnd::Root_wnd() {
  init_root_wnd();
}

void Root_wnd::init_root_wnd() {
  hpw::shmup_mode = true;
  graphic::camera = new_shared<Camera>();
  editor::anim_name = {};
  hpw::hitbox_layer = new_shared<Image>(graphic::canvas->X, graphic::canvas->Y);
  hpw::entity_mgr = new_unique<Entity_mgr>();
  hpw::anim_mgr = new_unique<Anim_mgr>();
  load_resources();
  load_animations();
  wnds.clear();
  wnds.emplace_back(new_shared<Opts_wnd>());
  wnds.emplace_back(new_shared<Menu_wnd>());
  wnds.emplace_back(new_shared<Emit_wnd>());
  wnds.emplace_back(new_shared<Anim_wnd>());
  wnds.emplace_back(new_shared<Frame_wnd>());
} // init_root_wnd

void Root_wnd::draw(Image& dst) const {
  draw_bg(dst);
  if (editor::use_draw_cross)
    draw_cross_bg(dst);
  cppfor(wnds, draw(dst));
  hpw::entity_mgr->draw(*graphic::canvas, graphic::camera->get_offset());
  if (editor::use_draw_cross && (graphic::frame_count & 0b100))
    draw_cross_fg(dst);
  if (editor::is_pause)
    draw_pause(dst);
  if (graphic::draw_hitboxes) // показать хитбоксы
    draw_hitboxes(dst);
  if (editor::use_zoom_x2)
    draw_zoom_x2(dst);

  if (editor::use_draw_frames)
    draw_frames(editor::entity, dst, {15, 15});
} // draw

void Root_wnd::update(const Delta_time dt) { 
  if (editor::is_reset) {
    init_root_wnd();
    editor::is_reset = false;
  }

  cppfor(wnds, update(dt));
  if ( !editor::is_pause)
    hpw::entity_mgr->update(dt);
  
  other_bg_pos += dt * 30;
}

void Root_wnd::imgui_exec() { cppfor(wnds, imgui_exec()) }

void Root_wnd::draw_zoom_x2(Image& dst) const
  { insert_x2(dst, dst, -Vec{dst.X/2, dst.Y/2}); }

void Root_wnd::draw_cross_bg(Image& dst) const {
  auto center = floor( center_point(dst) );
  auto col {Pal8::white};
  cfor (x, dst.X)
    dst.fast_set<&blend_diff>(std::floor(x), std::floor(center.y), col);
  cfor (y, dst.Y)
    dst.fast_set<&blend_diff>(std::floor(center.x), std::floor(y), col);
}

void Root_wnd::draw_cross_fg(Image& dst) const {
  auto center = floor( center_point(dst) );
  draw_cross<&blend_diff>(dst, center, Pal8::white, 4);
}

void Root_wnd::draw_bg(Image& dst) const {
  // фон с узором
  if (editor::other_bg) {
    #pragma omp parallel for simd collapse(2)
    cfor (y, dst.Y)
    cfor (x, dst.X) {
      auto col = (x + scast<int>(std::floor(other_bg_pos))) | y;
      col = blend_sub_safe(Pal8::from_real(editor::bg_color_val), col);
      dst(x, y) = col;
    }
  } else { // обычный одноцветный фон
    auto bg_color = Pal8::from_real(
      editor::bg_color_val, editor::bg_color_red);
    dst.fill(bg_color);
  }
}

void Root_wnd::draw_pause(Image& dst) const {
  auto pause_str = U"ПАУЗА";
  auto w = graphic::font->text_width(pause_str);
  auto h = graphic::font->text_height(pause_str);
  graphic::font->draw(dst, Vec(dst.X/2 - w/2, dst.Y - h - 15),
    pause_str, &blend_xor);
}

void Root_wnd::draw_hitboxes(Image& dst) const {
  // мигание
  return_if(graphic::frame_count & 0b100);
  insert<&blend_diff_no_black>(dst, *hpw::hitbox_layer, {});
  hpw::hitbox_layer->fill(Pal8::black);
}
