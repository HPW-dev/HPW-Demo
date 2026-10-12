#include "pch.hpp"
#include "scene-subpixel-test.hpp"
#include "game/core/messages.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/input/keys.hpp"
#include "engine/graphic/util/graphic-util.hpp"
#include "engine/graphic/util/util-templ.hpp"
#include "engine/graphic/util/resize.hpp"

struct Scene_subpixel_test::Impl {
  enum class Scale { none, x2, x3, x4, SZ };
  Scale _scale {Scale::x2};
  constx real _star_scale {55};
  constx Vec _star_center {512/4.0, 384/4.0};
  Vec _star_pos {};
  real _star_deg {};
  real _state {};
  real _speed {0.3};
  mutable Image _buffer {};
  mutable Image _down_buffer {};

  inline Impl() {
    // показать управление:
    hpw::message_mgr->move(Message { .text=
      U"Управление:\n"
      U"  " + hpw::keys_info.find(hpw::keycode::mode)->name + U" - Изменить масштабирование\n"
      U"  " + hpw::keys_info.find(hpw::keycode::up)->name + U"/"
        + hpw::keys_info.find(hpw::keycode::down)->name + U" - Увел./Умень. скорость движения\n"
    });
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape)) hpw::scene_mgr.back();
    if (is_pressed_once(hpw::keycode::mode)) next_scale();
    if (is_pressed_once(hpw::keycode::up)) speed_up();
    if (is_pressed_once(hpw::keycode::down)) speed_down();

    _state += dt;
    _star_pos = _star_center + Vec(
      std::cos(_state) * 10 * _speed,
      std::sin(_state) * 10 * _speed);
    _star_deg += dt * 2 * _speed;
  }
  
  inline void speed_up() {
    _speed += 0.1;
    hpw::message_mgr->move(Message{ .text=U"Скорость: " + n2s<utf32>(_speed, 1) });
  }
  inline void speed_down() {
    _speed = std::max<real>(0, _speed - 0.1);
    hpw::message_mgr->move(Message{ .text=U"Скорость: " + n2s<utf32>(_speed, 1) });
  }

  inline static utf32 to_name(Scale s) {
    switch(s) {
      default:
      case Scale::none: return U"1:1";
      case Scale::x2: return U"x2";
      case Scale::x3: return U"x3";
      case Scale::x4: return U"x4";
    }
  }

  inline void next_scale() {
    _scale = scast<Scale>((scast<int>(_scale) + 1) % scast<int>(Scale::SZ));
    hpw::message_mgr->move(Message{ .text=U"Масштабирование: " + to_name(_scale) });
  }

  inline static void draw_5way_star(Image& dst, Vec center, real scale, real degree) {
    Vector<Vec> points;
    cfor (i, 5) {
      const real d = (360.0 / 5.0) * i;
      const Vec p(0, scale);
      points.push_back(center + rotate_deg({}, p, d + degree));
    }

    const Pal8 c = Pal8::white;
    draw_line(dst, points[0], points[2], c);
    draw_line(dst, points[2], points[4], c);
    draw_line(dst, points[4], points[1], c);
    draw_line(dst, points[1], points[3], c);
    draw_line(dst, points[3], points[0], c);
  }

  inline void draw(Image& dst) const {
    real s;
    switch (_scale) {
      default:
      case Scale::none: s = 1.0; break;
      case Scale::x2: s = 2.0; break;
      case Scale::x3: s = 3.0; break;
      case Scale::x4: s = 4.0; break;
    }

    if (_scale != Scale::none) {
      // увеличенный рисунок
      _buffer.init(dst.X * s, dst.Y * s, Pal8::black);
      // повторить несколько раз звезду, чтобы сделать контур толще
      cfor (sy, s)
      cfor (sx, s)
        draw_5way_star(_buffer, _star_pos * s + Vec(sx, sy),
          _star_scale * s, _star_deg);
      // сглаживание
      switch (_scale) {
        case Scale::x2: downscale_x2(_down_buffer, _buffer); break;
        case Scale::x3: downscale_x3(_down_buffer, _buffer); break;
        case Scale::x4: downscale_x4(_down_buffer, _buffer); break;
      }
      insert(dst, _down_buffer);
    } else {
      dst.fill({});
      draw_5way_star(dst, _star_pos, _star_scale, _star_deg);
    }
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_subpixel_test)
