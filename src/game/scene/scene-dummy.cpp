#include "pch.hpp"
#include "scene-dummy.hpp"
#include "engine/graphic/util/graphic-util.hpp"
#include "engine/graphic/util/util-templ.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/util/keybits.hpp"

struct Scene_dummy::Impl {
  mutable Image test_pic;

  inline explicit Impl() {
    test_pic.init(*graphic::canvas);
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
    
    // TODO
  }

  inline void draw(Image& dst) const {
    cauto gsz = 8; // grid size

    test_pic.fill({});
    draw_grid(test_pic, gsz);
    draw_h_gradient(test_pic, gsz * 2, gsz * 2, gsz * 10, gsz * 2);

    insert_fast(dst, test_pic);
  }

  inline void draw_grid(Image& dst, int grid_sz) const {
    cauto c = Pal8::from_real(0.2);
    cfor (y, dst.Y / grid_sz)
      draw_line(dst, Vec(0, y * grid_sz), Vec(dst.X, y * grid_sz), c);
    cfor (x, dst.X / grid_sz)
      draw_line(dst, {x * grid_sz, 0}, {x * grid_sz, dst.Y}, c);
  }

  inline void draw_h_gradient(Image& dst, int ox, int oy, int w, int h) const {
    cfor (y, h)
    cfor (x, w) {
      auto c = Pal8::white;
      dst[ox + x, oy + y] = c;
    }
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_dummy)
