#include <omp.h>
#include <cassert>
#include "level-debug.hpp"
#include "graphic/font/font.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/animation/animation-manager.hpp"
#include "game/core/canvas.hpp"
#include "game/util/sync.hpp"
#ifdef CLD_DEBUG
#include "game/core/debug.hpp"
#endif
#include "game/entity/entity-manager.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/hitbox.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "game/entity/util/entity-util.hpp"
//#include "game/entity/collider/collider-simple.hpp"
#include "game/entity/collider/collider-qtree.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "game/core/entities.hpp"
#include "graphic/animation/anim.hpp"
#include "util/math/random.hpp"
#include "util/math/vec-util.hpp"

Level_debug::Level_debug() {
  hitbox_test();
  //hpw::entity_mgr->set_collider(  new_shared<Collider_simple>() );
  hpw::entity_mgr->set_collider( new_shared<Collider_qtree>(6, 1, graphic::canvas->X, graphic::canvas->Y) );
  set_rnd_seed(97997);

  // сделать рандомные объекты
  //cfor (_, 7'000) {
  cfor (_, 2'000) {
  //cfor (_, 100) {
    auto entity = hpw::entity_mgr->allocate<Collidable>();

    // позиция объекта
    entity->set_pos( Vec(
      rndr(0, graphic::width),
      rndr(0, graphic::height)
    ) );
    entity->phys.set_deg(rand_degree_stable());
    add_anim(*scast<Entity*>(entity), "anim.bullet.small.blinker.1");
    entity->anim_ctx.set_speed_scale(rndr());
    entity->anim_ctx.randomize_cur_frame_safe();
    entity->move_update_callback( &bounce_off_screen );
    entity->set_hp(100);
    entity->set_dmg(0);

    #if 1
      // двигаться
      entity->phys.set_vel( Vec(
        rndr(-2*60, 2*60),
        rndr(-2*60, 2*60)
      ) );
    #endif
  } // for objects
} // c-tor

void Level_debug::update(const Vec vel, double dt) {
  brightness += 20 * dt;
  
  print_collision_info();
} // update

void Level_debug::draw(Image& dst) const {
  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    Pal8 col(x & y);
    col = blend_add(col, brightness);
    col = blend_sub_safe(127, col);
    dst(x, y) = col;
  }
} // draw

void Level_debug::print_collision_info() {
#ifdef CLD_DEBUG
  Str txt;
  txt += "circle check: " + n2s<Str>(hpw::circle_checks);
  txt += ", poly: " + n2s<Str>(hpw::poly_checks);
  txt += ", collided: " + n2s<Str>(hpw::total_collided);
  hpw_log(txt << "\n");
  hpw::circle_checks = 0;
  hpw::poly_checks = 0;
  hpw::total_collided = 0;
#endif
} // print_collision_info

void Level_debug::hitbox_test() {
  #ifdef DEBUG
  hpw_log("старт тестов хитбоксов\n");

  { // пустой
    Hitbox empty;
    assert( !empty );
  }
  { // любое значение оффсета
    Hitbox hitbox;
    hitbox.polygons.emplace_back(Polygon {
      .offset = Vec(1, 7)
    });
    assert(hitbox);
  }
  { // любое значение полигона
    Hitbox hitbox;
    hitbox.polygons.emplace_back(Polygon {
      .points = {Vec(7, -4)}
    });
    assert(hitbox);
  }
  { // равенство оффсетов
    Hitbox a, b;
    a.polygons.emplace_back(Polygon { .offset = Vec(1, 7) });
    b.polygons.emplace_back(Polygon { .offset = Vec(1, 7) });
    assert(a == b);
  }
  { // неравенство оффсетов
    Hitbox a, b;
    a.polygons.emplace_back(Polygon { .offset = Vec(1, 7) });
    b.polygons.emplace_back(Polygon { .offset = Vec(-1, 7) });
    assert(a != b);
  }
  { // равенство точек
    Hitbox a, b;
    a.polygons.emplace_back(Polygon { .points = {Vec(1, 0), Vec(1, -4), Vec(24, 11)} });
    b.polygons.emplace_back(Polygon { .points = {Vec(1, 0), Vec(1, -4), Vec(24, 11)} });
    assert(a == b);
  }
  { // неравенство точек
    Hitbox a, b;
    a.polygons.emplace_back(Polygon { .points = {Vec(1, 0), Vec(1, -4), Vec(24, 11)} });
    b.polygons.emplace_back(Polygon { .points = {Vec(1, 0), Vec(3, -4), Vec(24, 11)} });
    assert(a != b);
  }
  { // горизонтальная симметрия
    Hitbox hitbox {
      .polygons = Polygons { {
        Polygon {
          .offset = {-10, 20},
          .points = {
            Vec(0, 20),
            Vec(24, 40),
            Vec(-80, -10),
          }
        }
      } }
    };
    // добавится новый хитбокс и он будет симметричным
    hitbox.make_h_symm();
    cnauto other_poly = hitbox.polygons.at(1);
    // проверить что реально симметричный
    assert(scast<int>(other_poly.offset.x) == 10);
    assert(scast<int>(other_poly.points.at(0).x) == 0);
    assert(scast<int>(other_poly.points.at(0).y) == 20);
    assert(scast<int>(other_poly.points.at(1).x) == -24);
    assert(scast<int>(other_poly.points.at(1).y) == 40);
    assert(scast<int>(other_poly.points.at(2).x) == 80);
    assert(scast<int>(other_poly.points.at(2).y) == -10);
  }
  { // проверка копирования
    Hitbox a {
      .polygons = Polygons { {
        Polygon {
          .offset = {-10, 20},
          .points = {
            Vec(0, 20),
            Vec(24, 40),
            Vec(-80, -10),
          }
        }
      } }
    };
    Hitbox b = a;
    assert(a == b);
  }

  hpw_log("все тесты хитбоксов пройдены\n");
  #endif
} // hitbox_test
