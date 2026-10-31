#include "pch.hpp"
#include "dbg-plots.hpp"
#include "plotter.hpp"
#include "game/core/fonts.hpp"
#include "game/core/core.hpp"
#include "game/util/sync.hpp"
#include "game/core/graphic.hpp"
#include "engine/graphic/image/image.hpp"
#include "engine/graphic/util/graphic-util.hpp"
#include "engine/graphic/util/util-templ.hpp"

struct Dbg_plots::Impl {
  Vector<Plotter> m_plotters {}; // графики, которые надо нарисовать

  inline Impl() {
    m_plotters = {
      Plotter(U"тик",            []->Plotter::Value { return hpw::tick_time; },          180, 60, 240*4, {5, 5}),
      Plotter(U"тик зум",        []->Plotter::Value { return hpw::tick_time; },          180, 60, 30,    {260, 5}),
      Plotter(U"софт. кадр",     []->Plotter::Value { return graphic::soft_draw_time; }, 180, 66, 240*6, {5, 75}),
      Plotter(U"софт. кадр зум", []->Plotter::Value { return graphic::soft_draw_time; }, 180, 66, 60,    {260, 75}),
      Plotter(U"хард. кадр",     []->Plotter::Value { return graphic::hard_draw_time; }, 180, 66, 240*6, {5, 150}),
      Plotter(U"хард. кадр зум", []->Plotter::Value { return graphic::hard_draw_time; }, 180, 66, 30,    {260, 150}),
      Plotter(U"FPS",            []->Plotter::Value { return graphic::cur_fps; },        180, 60, 240*6, {5, 225}),
    };
  }

  inline void draw(Image& dst) const {
    apply_brightness(dst, -97);
    
    for (crauto plotter: m_plotters)
      plotter.draw(dst);
  }

  inline void update(Delta_time dt) {
    for (rauto plotter: m_plotters)
      plotter.update();
  }
}; // Impl

Dbg_plots::Dbg_plots(): impl{new_unique<Impl>()} {}
Dbg_plots::~Dbg_plots() {}
void Dbg_plots::draw(Image& dst) const { impl->draw(dst); }
void Dbg_plots::update(Delta_time dt) { impl->update(dt); }

void draw_fps_info(Image& dst) {
  // прогнозируемая производительность FPS без VSync
  cauto GAME_PERFOMANCE = safe_div(1.0,
    std::clamp<real>(hpw::tick_time + graphic::soft_draw_time, 0.0002, 10));
  static Average<real, 60> game_perfomance_avr;
  game_perfomance_avr.push(GAME_PERFOMANCE);

  std::stringstream txt;
  txt <<   "Dt real         " << n2s(hpw::real_dt, 7);
  txt << "   Safe  " << n2s(hpw::safe_dt, 7);
  txt << "\nDraw time Soft. " << n2s(graphic::soft_draw_time, 7);
  txt << "   Hard. " << n2s(graphic::hard_draw_time, 7);
  txt << "\nTick time       " << n2s(hpw::tick_time, 7);
  txt << "   UPS   " << n2s(hpw::cur_ups, 1);
  txt << "\nFPS             " << n2s(graphic::cur_fps, 1);
  txt << "\nPerfomance      " << n2s(GAME_PERFOMANCE, 1);
  txt << "\nPerfomance avr. " << n2s(std::floor(game_perfomance_avr()), 0);
  txt << "\nRender lag      [" << (graphic::render_lag ? '#' : ' ') << "]";

  auto str_u32 = sconv<utf32>(txt.str());
  // нарисовать чёрный прямоуг под текст
  auto pos_x = 20;
  auto pos_y = 20;
  auto border = 8;
  static int w; // чтоб по ширине не дёргалось окно
  w = std::max(w, graphic::system_mono->text_width(str_u32));
  int h = graphic::system_mono->text_height(str_u32);
  Rect rect {
    pos_x - border,
    pos_y - border,
    w + border + 6,
    h + border + 12
  };
  
  draw_rect_filled<&blend_readable>(dst, rect, Pal8::black);
  draw_rect<&blend_diff>(dst, rect, Pal8::white);
  graphic::system_mono->draw(dst, {pos_x, pos_y}, str_u32);
} // draw_debug_info
