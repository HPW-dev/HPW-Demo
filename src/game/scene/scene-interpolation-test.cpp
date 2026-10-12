#include "pch.hpp"
#include "scene-interpolation-test.hpp"
#include "game/core/messages.hpp"
#include "game/core/sprites.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/input/keys.hpp"
#include "engine/graphic/util/graphic-util.hpp"
#include "engine/graphic/util/util-templ.hpp"
#include "engine/graphic/util/resize.hpp"

struct Scene_interpolation_test::Impl {
  Image _img {};
  real _scale_speed = 0.04;
  real _scale {0.5};
  using Resizer = Image (*)(cr<Image>, const uint, const uint);
  Resizer _resizer {};
  uint _resizer_id {1};
  bool _zoom_out = false;

  inline Impl() {
    load_img();
    print_inputs();
    next_resizer(0);
  }

  inline void load_img() {
    auto spr = hpw::sprites.find("graphic/images/misc/test images/complex test 2.png");
    iferror(!spr, "sprite not finded");
    _img = spr->image();
  }

  inline void print_inputs() {
    hpw::message_mgr->move(Message{ .text=
      U"Управление:\n"
      U"  " + hpw::keys_info.find(hpw::keycode::left)->name + U"/"
        + hpw::keys_info.find(hpw::keycode::right)->name + U" - Алгоритм ресайза\n"
      U"  " + hpw::keys_info.find(hpw::keycode::up)->name + U"/"
        + hpw::keys_info.find(hpw::keycode::down)->name + U" - Увел./Умень. скорость зума\n",
      .bf = &blend_diff
    });
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::left)) next_resizer(-1);
    if (is_pressed_once(hpw::keycode::right)) next_resizer(+1);
    if (is_pressed_once(hpw::keycode::up)) add_speed(+0.01);
    if (is_pressed_once(hpw::keycode::down)) add_speed(-0.01);
    if (is_pressed_once(hpw::keycode::escape)) hpw::scene_mgr.back();
    
    update_zoom(dt);
  }

  inline void next_resizer(int offset) {
    struct Item {
      Resizer resizer {};
      utf32 msg {};
    };
    sconst Vector<Item> table {
      Item {.resizer=&resize_neighbor, .msg=U"Ресайзер: ближайший сосед"},
      Item {.resizer=&resize_bilinear, .msg=U"Ресайзер: билинейный"},
      Item {.resizer=&resize_area, .msg=U"Ресайзер: область"},
    };
    _resizer_id = (_resizer_id + offset) % table.size();
    _resizer = table[_resizer_id].resizer;
    hpw::message_mgr->move(Message{ .text=table[_resizer_id].msg });
  }

  inline void add_speed(real value) {
    _scale_speed = std::max<real>(0, _scale_speed + value);
    hpw::message_mgr->move(Message{ .text=U"Скорость: " + n2s<utf32>(_scale_speed, 2) });
  }

  inline void update_zoom(const Delta_time dt) {
    if (_zoom_out) {
      _scale -= _scale_speed * dt;
      if (_scale <= 0) {
        _zoom_out = false;
        _scale = 0;
      }
    } else {
      _scale += _scale_speed * dt;
      if (_scale >= 1.0) {
        _zoom_out = true;
        _scale = 1;
      }
    }
  }

  inline static real smooth_zoom(real t, real min_zoom, real max_zoom) {
    return min_zoom * std::pow(max_zoom / min_zoom, t);
  }

  inline void draw(Image& dst) const {
    dst.fill({});

    iferror(!_resizer, "resizer is null");
    cauto resized = _resizer(_img,
      std::round(_img.X * smooth_zoom(_scale, 0.1, 2)),
      std::round(_img.Y * smooth_zoom(_scale, 0.1, 2)));
    insert(dst, resized, center_point(dst, resized));
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_interpolation_test)
