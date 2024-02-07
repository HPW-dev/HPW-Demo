#include <omp.h>
#include "collider-simple.hpp"
#include "game/entity/collidable.hpp"
#include "game/game-debug.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "graphic/util/util-templ.hpp"

void Collider_simple::test_collide(Entity& a, Entity& b) {
  // проверить что можно сталкивать
  if (a.status.collidable && b.status.collidable) {
    // проверить что объекты живые
    return_if(!a.status.live);
    return_if(!b.status.live);
    // сталкиваемые объекты можно смело кастовать
    auto a_collidable = scast<Collidable*>(&a);
    auto b_collidable = scast<Collidable*>(&b);
    // обновление флага столкновений
    bool collided = cld_flag_compat(a, b)
      ? a_collidable->is_collided_with(*b_collidable)
      : false;
    a_collidable->status.collided |= collided;
    b_collidable->status.collided |= collided;
  }
} // test_collide

void Collider_simple::operator()(CN<Entitys> entities, double dt) {
  cauto entitys_sz = entities.size();

  // проверить столкновения пар объектов (без повторений)
  #pragma omp parallel for simd schedule(dynamic, 4)
  for (  std::size_t a_i = 0;       a_i < entitys_sz - 1; ++a_i) {
    for (std::size_t b_i = a_i + 1; b_i < entitys_sz;     ++b_i)
      test_collide(*entities[a_i], *entities[b_i]);
  }
} // processing
