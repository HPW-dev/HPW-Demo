#ifdef DEBUG
#include <cassert>
#include <iostream>
#include "level-replay-test.hpp"
#include "util/math/vec.hpp"
#include "util/math/random.hpp"
#include "util/math/timer.hpp"
#include "util/vector-types.hpp"
#include "util/hpw-util.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "game/entity/collidable.hpp"
#include "game/util/keybits.hpp"
#include "game/core/scenes.hpp"
#include "game/core/entities.hpp"
#include "game/core/core.hpp"
#include "game/core/canvas.hpp"
#include "game/scene/scene-manager.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"

struct Level_replay_test::Impl {
  uint m_proto_counter {};
  uint m_post_counter {};
  uint m_update_count {};
  uint m_lifetime {};
  constx uint m_lifetime_max = 2'000;
  Timer m_timer_test {2.5}; /// срабатывает и пишет о себе в лог
  Timer m_timer_test_2 {0.001}; /// чаще срабатывает чем m_timer_test
  Entity* m_actor {};

  ~Impl() = default;

  inline Impl() {
    std::cerr << "seed: " << get_rnd_seed() << "\n\n";
    spawn_bouncers();
    spawn_bouncers_2();
    init_actor();
    init_player();
  }

  inline void update(const Vec vel, double dt) {
    assert(dt == hpw::target_update_time);
    std::cerr << "update count: " << m_update_count++;

    test_proto_rand();
    test_mixed_rand();
    test_timer(dt);
    test_bouncers(dt);
    //move_actor();
    save_input();
    test_post_rand();

    std::cerr << std::endl;
    if (m_lifetime++ >= m_lifetime_max)
      hpw::scene_mgr->back();
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    draw_load_bar(dst, Vec(15, 350));
  }

  inline void test_proto_rand() {
    std::cerr << "\nproto counter: " << m_proto_counter++ << '\n';
    std::cerr << "proto rnd: " << rnd() << '\n';
    std::cerr << "proto rndb: " << +rndb() << '\n';
    std::cerr << "proto rndr: " << +rndr() << '\n';
    std::cerr << "proto rndu: " << +rndu() << '\n';
  }

  inline void test_post_rand() {
    std::cerr << "\npost counter: " << m_post_counter++ << '\n';
    std::cerr << "post rnd: " << rnd() << '\n';
    std::cerr << "post rndb: " << +rndb() << '\n';
    std::cerr << "post rndr: " << +rndr() << '\n';
    std::cerr << "post rndu: " << +rndu() << '\n';
  }

  inline void draw_load_bar(Image& dst, const Vec pos) const {
    Rect bar (pos.x, pos.y, 200, 9); // размер рамки полоски
    draw_rect<&blend_diff>(dst, bar, Pal8::white);
    // размер полоски загрузки
    real load_value = m_lifetime / scast<double>(m_lifetime_max);
    load_value *= (bar.size.x - 2);
    Rect load_rect(pos.x+1, pos.y+1, load_value, bar.size.y - 2);
    draw_rect_filled<&blend_diff>(dst, load_rect, Pal8::white);
  }

  inline void test_timer(double dt) {
    cfor (_, m_timer_test.update(dt))
      std::cerr << "timer_test activated" << '\n';
    cfor (_, m_timer_test_2.update(dt))
      std::cerr << "timer_test_2 activated" << '\n';
  }

  inline void test_mixed_rand() {
    cfor (_, rndu_fast(4)) { do_not_optimize(rndb_fast()); }
    std::cerr << "mixed rndb: " << +rndb() << '\n';
    cfor (_, rndu_fast(4)) { do_not_optimize(rndr_fast()); }
    std::cerr << "mixed rndr: " << rndr() << '\n';
    cfor (_, rndu_fast(4)) { do_not_optimize(rndu_fast()); }
    std::cerr << "mixed rndu: " << rndu() << '\n';
    cfor (_, rndu_fast(4)) { do_not_optimize(rnd_fast()); }
    std::cerr << "mixed rnd: " << rnd() << '\n';
  }

  inline void test_bouncers(double dt) {
    for (uint idx {}; auto entity: hpw::entity_mgr->get_entitys()) {
      if (entity->status.live) {
        std::cerr << "entity[" << idx << "]\n";
        std::cerr <<"  pos: " << to_str(entity->phys.get_pos()) << '\n';
        std::cerr <<"  vel: " << to_str(entity->phys.get_vel()) << '\n';
        std::cerr <<"  spd: " << entity->phys.get_speed() << '\n';
        std::cerr <<"  deg: " << entity->phys.get_deg() << '\n';
      }
      ++idx;
    }
  }

  inline void spawn_bouncers() {
    cfor (_, 100) {
      auto it = hpw::entity_mgr->allocate<Collidable>();
      add_anim(*it, "anim.bullet.sphere.gray");
      it->anim_ctx.blend_f = &blend_rotate_x4_safe;
      it->phys.set_pos(Vec(
        rndr(0, graphic::width),
        rndr(0, graphic::height)
      ));
      it->phys.set_speed( pps(rndr(0, 8)) );
      it->phys.set_deg( rand_degree_stable() );
    }
  }

  inline void spawn_bouncers_2() {
    cfor (_, 50) {
      auto it = hpw::entity_mgr->allocate<Collidable>();
      add_anim(*it, "anim.bullet.sphere.red");
      it->anim_ctx.blend_f = &blend_rotate_safe;
      it->phys.set_pos(Vec(
        rndr(0, graphic::width),
        rndr(0, graphic::height)
      ));
      it->phys.set_deg( rand_degree_stable() );
      it->phys.set_speed( pps(rndr(2, 6)) );
      it->move_update_callback( &bounce_off_screen );
    }
  }

  inline void save_input() {
    std::cerr << "input: ";
    std::cerr << (is_pressed(hpw::keycode::up)    ? 'U' : '_');
    std::cerr << (is_pressed(hpw::keycode::down)  ? 'D' : '_');
    std::cerr << (is_pressed(hpw::keycode::right) ? 'R' : '_');
    std::cerr << (is_pressed(hpw::keycode::left)  ? 'L' : '_');
    std::cerr << '\n';
  }

  inline void init_actor() {
    m_actor = hpw::entity_mgr->allocate<Collidable>();
    add_anim(*m_actor, "anim.enemy.delme");
    m_actor->set_pos( Vec(graphic::width/2.0, graphic::height/2.0) );
  }

  inline void move_actor() {
    nauto phys = m_actor->phys;
    phys.set_speed(0);
    if (is_pressed(hpw::keycode::up))
      phys.set_vel(phys.get_vel() + Vec(+0, -1));
    if (is_pressed(hpw::keycode::down))
      phys.set_vel(phys.get_vel() + Vec(+0, +1));
    if (is_pressed(hpw::keycode::left))
      phys.set_vel(phys.get_vel() + Vec(-1, +0));
    if (is_pressed(hpw::keycode::right))
      phys.set_vel(phys.get_vel() + Vec(+1, +0));
    if (auto vel = phys.get_vel(); vel)
      phys.set_vel( normalize_stable(vel) * 4_pps );
  }

  inline void init_player() {
    hpw::entity_mgr->make({}, "player.boo.dark",
      Vec(graphic::width/2.0, graphic::height/2.0) );
  }

}; // Impl

Level_replay_test::Level_replay_test(): impl {new_unique<Impl>()} {}
Level_replay_test::~Level_replay_test() {}
void Level_replay_test::update(const Vec vel, double dt) { impl->update(vel, dt); }
void Level_replay_test::draw(Image& dst) const { impl->draw(dst); }

#endif // DEBUG