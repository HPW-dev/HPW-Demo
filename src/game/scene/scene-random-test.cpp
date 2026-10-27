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
  size_t _rnd_gen_idx {0};
  Unique<Rnd_gen_base> _rnd_gen {};

  inline Impl() {
    print_inputs();
    set_gen(0);
  }

  inline void print_inputs() {
    hpw::message_mgr->move(Message{
      .text=U"Управление:\n  "
        + get_key_name(hpw::keycode::up) + U"/"
        + get_key_name(hpw::keycode::down) + U" - Сменить генератор рандома.\n",
      .bf = &blend_diff
    });
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape)) hpw::scene_mgr.back();
    if (is_pressed_once(hpw::keycode::up))     set_gen(-1);
    if (is_pressed_once(hpw::keycode::down))   set_gen(+1);
  }

  inline void draw(Image& dst) const {
    dst.fill({});
    // TODO
  }

  inline void set_gen(int idx_offset) {
    cauto gens = rnd_gens();
    assert(!gens.empty());
    int idx = _rnd_gen_idx + idx_offset;
    if (idx < 0)
      idx += gens.size();
    _rnd_gen_idx = idx % gens.size();
    cauto name = gens.at(_rnd_gen_idx);
    _rnd_gen = make_rnd_gen(name);

    hpw::message_mgr->move(Message{
      .text=U"Генератор: " + utf8_to_32(name), .bf=&blend_diff });
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_random_test)
