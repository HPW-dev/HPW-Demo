#include "pch.hpp"
#include "scene-dummy.hpp"
#include "engine/graphic/util/graphic-util.hpp"
#include "engine/graphic/util/util-templ.hpp"
#include "engine/graphic/util/resize.hpp"
#include "engine/graphic/util/fill.hpp"
#include "game/util/keybits.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"

struct Scene_dummy::Impl {
  mutable Image test_pic;
  int alpha = 128;

  inline explicit Impl() {
    test_pic.init(*graphic::canvas);
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
    
    if (is_pressed(hpw::keycode::up)) ++alpha;
    if (is_pressed(hpw::keycode::down)) --alpha;
    if (is_pressed(hpw::keycode::left)) alpha = 128;
  }

  inline void draw(Image& dst) const {
    cauto gsz = 8; // grid size

    test_pic.fill({});
    draw_grid(test_pic, gsz);

    draw_h_gradient(test_pic, gsz * 2, gsz * 2, gsz * 20, gsz * 2, false);
    draw_v_gradient(test_pic, gsz * 2, gsz * 5, gsz * 2, gsz * 17, false);
    draw_lines(test_pic, gsz * 5, gsz * 5, gsz * 17, gsz * 17);

    draw_circles(test_pic, gsz * 2, gsz * 24, gsz * 39, gsz * 19);
    draw_filtered(test_pic, gsz * 42, gsz * 24, gsz * 17, gsz * 17);
    draw_h_gradient(test_pic, gsz * 42, gsz * 41, gsz * 20, gsz * 2, true);
    draw_v_gradient(test_pic, gsz * 60, gsz * 24, gsz * 2, gsz * 16, true);
    
    draw_freq_pattern(test_pic, gsz * 23, gsz * 2, gsz * 39, gsz * 2);
    draw_noise(test_pic, gsz * 23, gsz * 5, gsz * 39, gsz * 4);
    draw_filled_lines(test_pic, gsz * 23, gsz * 10, gsz * 39, gsz * 12);

    draw_text(test_pic, gsz * 2, gsz * 44, 57);

    insert_fast(dst, test_pic);

    draw_rect_filled<blend_alpha>(dst, Rect(20, 20, 400, 300), Pal8::black, alpha);
    graphic::font->draw(dst, {30, 30}, U"Alpha : " + n2s<utf32>(alpha));
    graphic::font->draw(dst, {35, 45}, U"AbCdEf\n123456\n--__-.\n|||\\");
  }

  inline void draw_grid(Image& dst, int grid_sz) const {
    cauto c = Pal8::from_real(0.2);
    cfor (y, dst.Y / grid_sz)
      draw_line(dst, Vec(0, y * grid_sz), Vec(dst.X, y * grid_sz), c);
    cfor (x, dst.X / grid_sz)
      draw_line(dst, {x * grid_sz, 0}, {x * grid_sz, dst.Y}, c);
  }

  inline void draw_h_gradient(Image& dst, int ox, int oy, int w, int h, bool red) const {
    cfor (y, h)
    cfor (x, w) {
      auto c = Pal8::from_real(x / real(w), red);
      dst[ox + x, oy + y] = c;
    }
  }

  inline void draw_v_gradient(Image& dst, int ox, int oy, int w, int h, bool red) const {
    cfor (y, h)
    cfor (x, w) {
      auto c = Pal8::from_real(y / real(h), red);
      dst[ox + x, oy + y] = c;
    }
  }

  inline void draw_lines(Image& dst, int ox, int oy, int w, int h) const {
    cauto step = 20;
    cauto c = Pal8::white;

    draw_rect_filled(dst, {ox, oy, w, h}, Pal8::black);

    cfor (y, step)
      draw_line(dst, {ox, oy}, {ox + w, oy + y * (h / real(step))}, c);
    cfor (x, step+1)
      draw_line(dst, {ox, oy}, {ox + x * (w / real(step)), oy + h}, c);
    
    draw_rect(dst, {ox, oy, w+1, h+1}, c);
  }

  inline void draw_filtered(Image& dst, int ox, int oy, int w, int h) const {
    cauto step = 20;
    cauto scale = 3;
    cauto c = Pal8::red;
    cauto ws = w * scale;
    cauto hs = h * scale;

    draw_rect_filled(dst, {ox, oy, w, h}, Pal8::black);

    Image buffer(w*scale, h*scale, Pal8::black);
    cfor (y, step)
      draw_line(buffer, {0, 0}, {ws, y * (hs / real(step))}, c);
    cfor (x, step+1)
      draw_line(buffer, {0, 0}, {x * (ws / real(step)), hs}, c);
    insert(dst, pixel_downscale_x3(buffer, Color_get_pattern::box, Color_compute::average), {ox, oy});
    
    draw_rect(dst, {ox, oy, w+1, h+1}, c);
  }

  inline void draw_circles(Image& dst, int ox, int oy, int w, int h) const {
    cauto c = Pal8::white;
    cauto circles = 60;
    cauto step = 5;
    const Vec p1(w / 2 - 100, h / 2 - 25);
    const Vec p2(w / 2 + 100, h / 2 + 25);

    draw_rect_filled<blend_readable>(dst, {ox, oy, w, h}, Pal8::black);

    Image buffer(w, h, Pal8::black);
    cfor (i, circles) {
      draw_circle(buffer, p1, 1 + i * step, c);
      draw_circle(buffer, p2, 1 + i * step, c);
    }
    insert(dst, buffer, {ox, oy}, blend_max);
    
    draw_rect(dst, {ox, oy, w+1, h+1}, c);
  }

  inline void draw_freq_pattern(Image& dst, int ox, int oy, int w, int h) const {
    cfor(y, h)
    cfor(x, w) {
      const bool ok = uint((x / real(w)) * x * 0.5) % 2;
      dst[ox + x, oy + y] = (!ok ? Pal8::white : Pal8::black);
    }
  }

  inline void draw_noise(Image& dst, int ox, int oy, int w, int h) const {
    cfor (y, h)
    cfor (x, w)
      dst[ox + x, oy + y] = rndb_fast() + y * x * x;
    draw_rect(dst, {ox, oy, w+1, h+1}, Pal8::red);
  }

  inline void draw_text(Image& dst, int ox, int oy, uint chars) const {
    utf32 str;
    str.resize(chars);
    for (auto& ch: str)
      ch = rndu_fast(1024);
    graphic::font->draw(dst, {ox, oy}, str);
  }

  inline void draw_filled_lines(Image& dst, int ox, int oy, int w, int h) const {
    cauto c = Pal8::red;

    draw_rect_filled(dst, {ox, oy, w, h}, Pal8::black);
    draw_rect(dst, {ox, oy, w+1, h+1}, c);
  
    cfor (_, 10) {
      const Vec p1(ox + rndu_fast(w), oy);
      const Vec p2(ox + rndu_fast(w), oy + h);
      draw_line(dst, p1, p2, c);
    }
    cfor (_, 5) {
      const Vec p1(ox,     oy + rndu_fast(h));
      const Vec p2(ox + w, oy + rndu_fast(h));
      draw_line(dst, p1, p2, c);
    }
    
    cfor (y, 3)
    cfor (x, 5)
      if (rndb_fast() & 1)
        fill(dst, {ox + x * w / 5.0, oy + y * h / 3.0}, Pal8::red);
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_dummy)
