#include "pch.hpp"
#include "scene-random-test.hpp"
#include "game/core/messages.hpp"
#include "game/core/sprites.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/input/keys.hpp"
#include "engine/graphic/util/graphic-util.hpp"
#include "engine/graphic/util/util-templ.hpp"
#include "engine/graphic/util/resize.hpp"

class Random_test {
public:
  virtual ~Random_test() = default;
  virtual void update() = 0;
};

struct Scene_random_test::Impl {
  Vector<Unique<Random_test>> _tests {};

  inline Impl() {
    print_inputs();
  }

  inline void print_inputs() {
    hpw::message_mgr->move(Message{ .text=
      U"Управление:\n  "
        + get_key_name(hpw::keycode::up) + U"/"
        + get_key_name(hpw::keycode::down) + U" - Сменить генератор рандома.\n",
      .bf = &blend_diff
    });
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape)) hpw::scene_mgr.back();
    
  }

  inline void draw(Image& dst) const {
    dst.fill({});
    // TODO
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_random_test)
